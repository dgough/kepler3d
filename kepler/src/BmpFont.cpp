#include "stdafx.h"
#include "BmpFont.hpp"
#include "FileSystem.hpp"
#include "StringUtils.hpp"
#include "Effect.hpp"
#include "RenderState.hpp"
#include "Image.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "App.hpp"

#include <regex>
#include <chrono>
#include <array>
#include <cctype>

using std::string;
using std::regex;

namespace kepler {

    // Disable OpenGL code to make it easier to profile font loading
    static constexpr bool DISABLE_GL = false;

    /// @Internal
    class BmpFontRenderer;
    static std::weak_ptr<BmpFontRenderer> g_bmpFontRenderer;

    static constexpr size_t VERTEX_COUNT = 6; // per quad
    static constexpr size_t VERTEX_SIZE = 4;
    static constexpr size_t MAX_LETTERS = 80; // max letters per batch draw.
    static constexpr size_t BUFFER_SIZE = VERTEX_SIZE * VERTEX_COUNT * MAX_LETTERS;

    static constexpr GLuint VERTEX_INDEX = 0;
    static constexpr char* vertSource =
        "#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n"
        "uniform mat4 u_projection;\n"
        "out vec2 v_texcoord;\n"
        "void main() {\n"
        "    v_texcoord = vertex.zw;\n"
        "    gl_Position = u_projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "}\n";

    static constexpr char* fragSource =
        "#version 330 core\n"
        "in vec2 v_texcoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 u_textColor;\n"
        "void main() {\n"
        "    color = vec4(u_textColor, 1.0) * texture(text, v_texcoord);\n"
        "}  \n";

    static constexpr int FIRST_PRINTABLE = 32;
    static constexpr int LAST_PRINTABLE = 255;

    /// Converts string to integer.
    /// Returns defaultValue is there is an error.
    static int to_i(const string& str, int defaultValue = 0) {
        try {
            return std::stoi(str);
        }
        catch (std::invalid_argument&) {
            return defaultValue;
        }
    }

    static bool readIntAfter(const std::string& subject, const char* target, int& num) {
        size_t i = subject.find(target);
        if (i != string::npos) {
            i += strlen(target);
            if (i < subject.length()) {
                num = std::atoi(subject.c_str() + i);
                return true;
            }
        }
        return false;
    }
    static bool readIntAfter(const std::string& subject, const char* target, float& f) {
        int n;
        if (readIntAfter(subject, target, n)) {
            f = static_cast<float>(n);
            return true;
        }
        return false;
    }

    /// Assigns source to destination without the surrounding double quotes.
    static void assignWithoutQuotes(string& destination, const string& source);

    /// @Internal
    class BmpFontException : public std::runtime_error {
    public:
        explicit BmpFontException(const string& message) : std::runtime_error(message) {}
    };

    /// @Internal
    struct Character {
        char ch;
        float x;
        float y;
        float width;
        float height;
        float xoffset;
        float yoffset;
        float xadvance;
        float s, t, s2, t2;
        //int page;
        //int chnl;

        Character() : ch(' '), x(0.f), y(0.f), width(0.f), height(0.f), xoffset(0.f), yoffset(0.f), xadvance(0.f), s(0.f), t(0.f), s2(0.f), t2(0.f) {}
    };

    // Private implementation
    /// @Internal
    class BmpFont::Impl {
        friend class BmpFont;
    public:
        Impl();
        ~Impl() noexcept;
        void drawText(const char* text, float x, float y, const glm::vec3& color);
        void loadFromFile(const char* path, std::ifstream& file);
        void loadTextures();
    private:
        void loadInfo(const string& line);
        void loadCommon(const string& line);
        void loadPage(const string& line);
        void loadChars(const string& line);
        void loadChar(const string& line);
        void loadKernings(const string& line);
        void loadKerning(const string& line);

        void addCharacter(const Character& ch);
        bool getCharacter(char ch, Character& character);

    private:
        float _scale;
        int _size;
        // bold or italics
        // charset?
        // unicode?
        int _stretchH;
        int _lineHeight;
        int _base;
        int _scaleW;
        int _scaleH;
        int _spacingX;
        int _spacingY;
        //int _pages;
        int _packed;
        string _face;
        string _baseDir;

        // TODO use std::vector instead
        std::vector<Character> _chars;
        std::vector<string> _pages;

        std::shared_ptr<BmpFontRenderer> _renderer;

        // Assume there is only one texture for now.
        TextureRef _texture;
    };

    ////////////////////////////////////////////////////////////////////////////////

    /// Holds objects used to draw a font, shared between all instances of BmpFont.
    /// @Internal
    class BmpFontRenderer {
    public:
        BmpFontRenderer();
        ~BmpFontRenderer() noexcept;

        /// Draws a batch of letters.
        void draw(const GLfloat* data, GLsizei count, const Texture* texture, const glm::vec3& color) const noexcept;

        BmpFontRenderer(const BmpFontRenderer&) = delete;
        BmpFontRenderer& operator=(const BmpFontRenderer&) = delete;
    public:
        GLuint _vao;
        GLuint _vbo;
        RenderState _state;
        EffectRef _effect;
        SamplerRef _sampler;
    };

    ////////////////////////////////////////////////////////////////////////////////

    BmpFont::Impl::Impl() : _scale(1.f), _size(0) {
        if (DISABLE_GL) {
            return;
        }
        if (auto renderer = g_bmpFontRenderer.lock()) {
            _renderer = renderer;
        }
        else {
            _renderer = std::make_shared<BmpFontRenderer>();
            g_bmpFontRenderer = _renderer;
        }
    }

    BmpFont::Impl::~Impl() noexcept {
    }

    void BmpFont::Impl::drawText(const char* text, float x, float y, const glm::vec3& color) {
        std::array<GLfloat, BUFFER_SIZE> buffer;
        GLsizei letter_count = 0;
        static constexpr size_t increment = VERTEX_COUNT * VERTEX_SIZE;
        auto dest = buffer.begin();

        for (const char* c = text; *c != '\0'; ++c) {
            Character ch;
            if (!getCharacter(*c, ch)) {
                continue;
            }
            GLfloat xpos = x + ch.xoffset * _scale;
            GLfloat ypos = y + ch.yoffset * _scale;
            GLfloat xpos2 = xpos + ch.width * _scale;
            GLfloat ypos2 = ypos + ch.height * _scale;
            x += ch.xadvance * _scale;
            if (std::isspace(ch.ch)) {
                continue;
            }

            std::array<GLfloat, VERTEX_COUNT * VERTEX_SIZE> vertices{
                xpos,  ypos2, ch.s,  ch.t2,
                xpos2, ypos,  ch.s2, ch.t,
                xpos,  ypos,  ch.s,  ch.t,
                xpos,  ypos2, ch.s,  ch.t2,
                xpos2, ypos2, ch.s2, ch.t2,
                xpos2, ypos,  ch.s2, ch.t
            };
            std::copy(vertices.begin(), vertices.end(), dest);
            dest += increment;
            ++letter_count;
            if (letter_count >= MAX_LETTERS) {
                _renderer->draw(buffer.data(), letter_count, _texture.get(), color);
                letter_count = 0;
                dest = buffer.begin();
            }
        }
        if (letter_count != 0) {
            _renderer->draw(buffer.data(), letter_count, _texture.get(), color);
        }
    }

    void BmpFont::Impl::loadFromFile(const char* path, std::ifstream& file) {
        _baseDir = getDirectoryName(path);

        string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            switch (line[0]) {
            case 'i':
                if (startsWith(line, "info")) {
                    loadInfo(line);
                }
                break;
            case 'p':
                if (startsWith(line, "page")) {
                    loadPage(line);
                }
                break;
            case 'c':
                if (startsWith(line, "common")) {
                    loadCommon(line);
                }
                else if (startsWith(line, "chars")) {
                    loadChars(line);
                }
                else if (startsWith(line, "char")) {
                    loadChar(line);
                }
                break;
            case 'k':
                if (startsWith(line, "kernings")) {
                    return;
                    //loadKerning(line);
                }
                else if (startsWith(line, "kerning")) {
                    //loadKernings(line);
                }
                break;
            default:
                break;
            }
        }
    }

    void BmpFont::Impl::loadTextures() {
        if (DISABLE_GL) {
            return;
        }
        if (_pages.size() < 1) {
            throw BmpFontException("no font pages found");
        }
        auto path = joinPath(_baseDir, _pages[0]);
        auto image = Image::createFromFile(path.c_str());
        if (image) {
            _texture = Texture::create2D(image.get(), GL_RGBA);
            _texture->setSampler(_renderer->_sampler);
        }
    }

    void BmpFont::Impl::loadInfo(const string& line) {
        readIntAfter(line, "size=", _size);
        readIntAfter(line, "stretchH=", _stretchH);
        // bold
        // charset
        // italic

        regex face_regex(" face=(\"[\\w\\s,.-]+\"|[\\w,.-]+)");
        std::smatch match;
        if (std::regex_search(line, match, face_regex) && match.size() > 1) {
            assignWithoutQuotes(_face, match.str(1));
        }

        regex spacing_regex(" spacing=([0-9-]+),([0-9-]+)");
        std::smatch spacingMatch;
        if (std::regex_search(line, spacingMatch, spacing_regex) && spacingMatch.size() > 2) {
            _spacingX = to_i(spacingMatch.str(1));
            _spacingY = to_i(spacingMatch.str(2));
        }
    }

    void BmpFont::Impl::loadCommon(const string& line) {
        readIntAfter(line, "lineHeight=", _lineHeight);
        readIntAfter(line, "base=", _base);
        readIntAfter(line, "scaleW=", _scaleW);
        readIntAfter(line, "scaleH=", _scaleH);
        readIntAfter(line, "packed=", _packed);

        int pageCount;
        if (readIntAfter(line, "pages=", pageCount)) {
            if (pageCount > 0) {
                _pages.resize(pageCount);
            }
        }
    }

    void BmpFont::Impl::loadPage(const string& line) {
        regex file_regex(" file=(\"[\\w\\s.-]+\"|[\\w.-]+)");

        int page_id = 0;
        readIntAfter(line, "id=", page_id);

        string filename;
        std::smatch match;
        if (std::regex_search(line, match, file_regex) && match.size() > 1) {
            assignWithoutQuotes(filename, match.str(1));
        }
        if (page_id < _pages.size()) {
            _pages[page_id].assign(filename);
        }
    }

    void BmpFont::Impl::loadChars(const string& line) {
        int count;
        if (readIntAfter(line, "count=", count)) {
            _chars.reserve(count);
        }
    }

    void BmpFont::Impl::loadChar(const string& line) {
        Character ch;

        int id;
        if (readIntAfter(line, "id=", id)) {
            if (id >= FIRST_PRINTABLE && id <= LAST_PRINTABLE) {
                ch.ch = static_cast<char>(id);
            }
            else {
                return;
            }
        }
        readIntAfter(line, "x=", ch.x);
        readIntAfter(line, "y=", ch.y);
        readIntAfter(line, "xoffset=", ch.xoffset);
        readIntAfter(line, "yoffset=", ch.yoffset);
        readIntAfter(line, "xadvance=", ch.xadvance);
        readIntAfter(line, "width=", ch.width);
        readIntAfter(line, "height=", ch.height);
        ch.s = (float)ch.x / (float)_scaleW;
        ch.s2 = (float)(ch.x + ch.width) / (float)_scaleW;
        ch.t = (float)ch.y / (float)_scaleH;
        ch.t2 = (float)(ch.y + ch.height) / (float)_scaleH;
        addCharacter(ch);
    }

    void BmpFont::Impl::loadKernings(const string& line) {
        // TODO
    }

    void BmpFont::Impl::loadKerning(const string& line) {
        // TODO
    }

    void BmpFont::Impl::addCharacter(const Character& ch) {
        size_t i = ch.ch;
        i &= 0xFF;
        i -= FIRST_PRINTABLE;
        if (i >= _chars.size()) {
            _chars.resize(i + 1);
        }
        _chars[i] = ch;
    }

    bool BmpFont::Impl::getCharacter(char ch, Character& character) {
        int i = ch - FIRST_PRINTABLE;
        if (i >= 0 && i < _chars.size()) {
            character = _chars[i];
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////

    BmpFontRenderer::BmpFontRenderer() : _vao(0), _vbo(0) {
        _effect = Effect::createFromSource(vertSource, fragSource);
        _effect->bind();
        glm::mat4 projection = glm::ortho(0.f, static_cast<GLfloat>(app()->getWidth()), static_cast<GLfloat>(app()->getHeight()), 0.f);
        _effect->setValue(_effect->getUniformLocation("u_projection"), projection);

        _state.setBlend(true);
        _state.setCulling(true);
        _state.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _sampler = Sampler::create();
        _sampler->setWrapMode(Sampler::Wrap::CLAMP_TO_EDGE, Sampler::Wrap::CLAMP_TO_EDGE);
        _sampler->setFilterMode(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR);

        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VERTEX_COUNT * VERTEX_SIZE * MAX_LETTERS, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(VERTEX_INDEX);
        glVertexAttribPointer(VERTEX_INDEX, VERTEX_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(GLfloat), 0);
        glBindVertexArray(0);
    }

    BmpFontRenderer::~BmpFontRenderer() noexcept {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
    }

    void BmpFontRenderer::draw(const GLfloat* data, GLsizei count, const Texture* texture, const glm::vec3& color) const noexcept {
        const auto* effect = _effect.get();
        effect->bind();
        _state.bind();
        effect->setValue(effect->getUniformLocation("u_textColor"), color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(_vao);
        texture->bind(0);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        static constexpr GLsizei SIZE = VERTEX_COUNT * VERTEX_SIZE * sizeof(GLfloat);
        glBufferSubData(GL_ARRAY_BUFFER, 0, SIZE * count, data);
        glDrawArrays(GL_TRIANGLES, 0, count * VERTEX_COUNT);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ////////////////////////////////////////////////////////////////////////////////

    BmpFont::BmpFont() : _impl(std::make_unique<Impl>()) {
    }

    BmpFont::~BmpFont() noexcept {
    }

    BmpFontRef BmpFont::createFromFile(const char* path) {
        if (path == nullptr || *path == '\0') {
            return nullptr;
        }
        try {
            std::ifstream file;
            file.exceptions(std::ifstream::badbit);
            file.open(path);
            if (!file) {
                std::clog << "ERROR::READ_TEXT_FILE " << path << std::endl;
                return false;
            }
            auto font = MAKE_SHARED(BmpFont);
            font->_impl->loadFromFile(path, file);
            file.close();
            font->_impl->loadTextures();
            return font;
        }
        catch (const std::ifstream::failure& e) {
            std::clog << "ERROR::READ_TEXT_FILE " << path << " " << e.what() << std::endl;
        }
        catch (const BmpFontException& e) {
            std::clog << "ERROR::LOAD_FONT " << path << " " << e.what() << std::endl;
        }
        return nullptr;
    }

    void BmpFont::drawText(const char* text, float x, float y, const glm::vec3& color) {
        _impl->drawText(text, x, y, color);
    }

    int BmpFont::getSize() const {
        return _impl->_size;
    }

    float BmpFont::getSizeAsFloat() const {
        return static_cast<float>(getSize());
    }

    int BmpFont::getLineHeight() const {
        return _impl->_lineHeight;
    }

    int BmpFont::getBase() const {
        return _impl->_base;
    }

    const std::string& BmpFont::getFace() const {
        return _impl->_face;
    }

    float BmpFont::getScale() const {
        return _impl->_scale;
    }

    void BmpFont::setScale(float scale) {
        _impl->_scale = scale;
    }

    void assignWithoutQuotes(string& destination, const string& source) {
        auto length = source.length();
        if (length >= 2 && source[0] == '"' && source[length - 1] == '"') {
            destination.assign(source.substr(1, length - 2));
        }
        else {
            destination.assign(source);
        }
    }
}
