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
        "out vec2 v_texCoords;\n"
        "void main() {\n"
        "    v_texCoords = vertex.zw;\n"
        "    gl_Position = u_projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "}\n";

    static constexpr char* fragSource =
        "#version 330 core\n"
        "in vec2 v_texCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 u_textColor;\n"
        "void main() {\n"
        "    color = vec4(u_textColor, 1.0) * texture(text, v_texCoords);\n"
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

    static float i_to_f(const string& str, int defaultValue = 0) {
        return static_cast<float>(to_i(str, defaultValue));
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
        void loadCommon(const string& line, const regex& re);
        void loadPage(const string& line);
        void loadChars(const string& line);
        void loadChar(const string& line, const regex& re);
        void loadKernings(const string& line, const regex& re);
        void loadKerning(const string& line, const regex& re);

        void readInfoValue(const string& key, const string& value);
        void readCommonValue(const string& key, const string& value);
        void readCharValue(const string& key, const string& value, Character& ch);

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
        std::map<char, Character> _chars;
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
            auto it = _chars.find(*c);
            if (it == _chars.end()) {
                continue;
            }
            auto& ch = it->second;
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
        regex keyEqualNumber("([a-zA-Z]+)=([0-9-]+)");
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
                    loadCommon(line, keyEqualNumber);
                }
                else if (startsWith(line, "chars")) {
                    loadChars(line);
                }
                else if (startsWith(line, "char")) {
                    loadChar(line, keyEqualNumber);
                }
                break;
            case 'k':
                if (startsWith(line, "kernings")) {
                    loadKerning(line, keyEqualNumber);
                }
                else if (startsWith(line, "kerning")) {
                    loadKernings(line, keyEqualNumber);
                }
                break;
            default:
                break;
            }
        }
    }

    void BmpFont::Impl::loadTextures() {
        if (_pages.size() < 1) {
            throw BmpFontException("no font pages found");
        }
        auto path = joinPath(_baseDir, _pages[0]);
        auto image = Image::createFromFile(path.c_str());
        if (image) {
            _texture = Texture::create2D(image, GL_RGBA);
            _texture->setSampler(_renderer->_sampler);

        }
    }

    void BmpFont::Impl::loadInfo(const string& line) {
        regex re("([a-zA-Z]+)=([0-9a-zA-Z.,-]+)");
        auto next(std::sregex_iterator(line.begin(), line.end(), re));
        //std::sregex_iterator end;
        //while (next != end) {
        //    string key = next->str(1);
        //    if (key.length() >= 2) {
        //        readInfoValue(key, next->str(2));
        //    }
        //    ++next;
        //}

        std::for_each(next, std::sregex_iterator(), [this](const std::smatch& match) {
            string key = match.str(1);
            if (key.length() >= 2) {
                readInfoValue(key, match.str(2));
            }
        });

        regex face_regex("\\bface=(\"[\\w\\s,.-]+\"|[\\w,.-]+)");
        std::smatch match;
        if (std::regex_search(line, match, face_regex) && match.size() > 1) {
            assignWithoutQuotes(_face, match.str(1));
        }

        regex spacing_regex("\\bspacing=([0-9-]+),([0-9-]+)");
        std::smatch spacingMatch;
        if (std::regex_search(line, spacingMatch, spacing_regex) && spacingMatch.size() > 2) {
            _spacingX = to_i(spacingMatch.str(1));
            _spacingY = to_i(spacingMatch.str(2));
        }
    }

    void BmpFont::Impl::loadCommon(const string& line, const regex& re) {
        auto next(std::sregex_iterator(line.begin(), line.end(), re));
        std::sregex_iterator end;
        while (next != end) {
            string key = next->str(1);
            if (key.length() >= 2) {
                readCommonValue(key, next->str(2));
            }
            ++next;
        }
    }

    void BmpFont::Impl::loadPage(const string& line) {
        regex id_regex("\\bid=([0-9]+)");
        regex file_regex("\\bfile=(\"[\\w\\s.-]+\"|[\\w.-]+)");

        int page_id = 0;
        string filename;

        std::smatch match;
        if (std::regex_search(line, match, id_regex) && match.size() > 1) {
            page_id = to_i(match.str(1));
        }
        if (std::regex_search(line, match, file_regex) && match.size() > 1) {
            assignWithoutQuotes(filename, match.str(1));
        }

        if (page_id < _pages.size()) {
            _pages[page_id].assign(filename);
        }
    }

    void BmpFont::Impl::loadChars(const string& line) {
        // TODO use a vector instead of a map and set its size here
        //regex re("count=([0-9]+)");
        //std::smatch match;
        //if (std::regex_search(line, match, re) && match.size() > 1)  {
        //    string n = match.str(1);
        //}
    }

    void BmpFont::Impl::loadChar(const string& line, const regex& re) {
        Character ch;
        auto next(std::sregex_iterator(line.begin(), line.end(), re));
        std::sregex_iterator end;
        while (next != end) {
            string key = next->str(1);
            if (!key.empty()) {
                readCharValue(key, next->str(2), ch);
            }
            ++next;
        }
        //std::for_each(next, std::sregex_iterator(), [this, &ch](const std::smatch& match) {
        //    string key = match.str(1);
        //    if (key.length() >= 2) {
        //        readCharValue(key, match.str(2), ch);
        //    }
        //});
        ch.s = (float)ch.x / (float)_scaleW;
        ch.s2 = (float)(ch.x + ch.width) / (float)_scaleW;
        ch.t = (float)ch.y / (float)_scaleH;
        ch.t2 = (float)(ch.y + ch.height) / (float)_scaleH;
        _chars[ch.ch] = ch;
    }

    void BmpFont::Impl::loadKernings(const string& line, const regex& re) {
        // TODO
    }

    void BmpFont::Impl::loadKerning(const string& line, const regex& re) {
        // TODO
    }

    void BmpFont::Impl::readInfoValue(const string& key, const string& value) {
        switch (key[0]) {
        case 'a':
            break;
        case 'b':
            // bold
            break;
        case 'c':
            // charset
            break;
        case 'i':
            // italic
            break;
        case 'p':
            break;
        case 's':
            switch (key.length()) {
            case 4:
                if (key == "size") {
                    int size = to_i(value);
                    if (size > 0) {
                        _size = size;
                    }
                }
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                if (key == "stretchH") {
                    _stretchH = to_i(value, 100);
                }
                break;
            default:
                break;
            }
            break;
        case 'u':
            break;
        }
    }

    void BmpFont::Impl::readCommonValue(const string& key, const string& value) {
        switch (key[0]) {
        case 'l':
            if (key == "lineHeight") {
                _lineHeight = to_i(value);
            }
            break;
        case 'b':
            if (key == "base") {
                _base = to_i(value);
            }
            break;
        case 's':
            if (key == "scaleW") {
                _scaleW = to_i(value);
            }
            else if (key == "scaleH") {
                _scaleH = to_i(value);
            }
            break;
        case 'p':
            if (key == "pages") {
                int pageCount = to_i(value);
                // TODO throw exception if > 1?
                if (pageCount > 0) {
                    _pages.resize(pageCount);
                }
            }
            else if (key == "packed") {
                _packed = to_i(value);
            }
            break;
        default:
            break;
        }
    }

    void BmpFont::Impl::readCharValue(const string& key, const string& value, Character& ch) {
        switch (key[0]) {
        case 'i':
            if (key == "id") {
                int id = to_i(value);
                if (id >= FIRST_PRINTABLE && id <= LAST_PRINTABLE) {
                    ch.ch = static_cast<char>(id);
                }
            }
            break;
        case 'x':
            if (key.length() == 1) {
                ch.x = i_to_f(value);
            }
            else if (key == "xoffset") {
                ch.xoffset = i_to_f(value);
            }
            else if (key == "xadvance") {
                ch.xadvance = i_to_f(value);
            }
            break;
        case 'y':
            if (key.length() == 1) {
                ch.y = i_to_f(value);
            }
            else if (key == "yoffset") {
                ch.yoffset = i_to_f(value);
            }
            break;
        case 'w':
            if (key == "width") {
                ch.width = i_to_f(value);
            }
            break;
        case 'h':
            if (key == "height") {
                ch.height = i_to_f(value);
            }
            break;
        case 'p':
            break;
        case 'c':
            break;
        default:
            break;
        }
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
