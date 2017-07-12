/// Lazy glTF 2.0 loader: https://github.com/dgough/lazy-gltf2
/// glTF 2.0 spec: https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
#pragma once
#ifndef LAZY_GLTF2_HPP
#define LAZY_GLTF2_HPP

// suppress warning about fopen_s
#define _CRT_SECURE_NO_WARNINGS

#include "lib64.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <vector>
#include <array>
#include <memory>
#include <cstdint>
#ifndef _WIN32
#include <libgen.h>
#endif

#define LAZY_GLTF2_DATA_APP_BASE64 "data:application/octet-stream;base64,"
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson { typedef ::std::size_t SizeType; }

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>

#ifndef LAZY_GLTF2_NAMESPACE
#define LAZY_GLTF2_NAMESPACE gltf2
#endif
namespace LAZY_GLTF2_NAMESPACE {

    class Node;
    class Camera;
    class Mesh;
    class Primitive;
    class Accessor;
    class Scene;
    class Asset;
    class Buffer;
    class BufferView;
    class Sparse;
    class Texture;
    class TextureInfo;
    class Sampler;
    class Animation;
    class Channel;
    class Material;
    class MorphTarget;
    class Image;
    class Skin;
    class GlbData;
    class Gltf;
    class Channel;

    struct FileCloser {
        void operator()(FILE* fp) {
            if (fp != nullptr) {
                ::std::fclose(fp);
            }
        }
    };
    using JsonDocument = ::rapidjson::Document;
    using JsonValue = ::rapidjson::Document::GenericValue;
    using unique_file_ptr = ::std::unique_ptr<FILE, FileCloser>;

    /// Animation target path.
    enum class TargetPath {
        TRANSLATION,
        ROTATION,
        SCALE,
        WEIGHTS
    };

    /// Animation sampler interpolation.
    enum class Interpolation {
        LINEAR,
        STEP,
        CATMULLROMSPLINE,
        CUBICSPLINE
    };

    static constexpr float DEFAULT_ASPECT_RATIO = 16.0f / 9.0f;
    /// First 4 bytes of GLB file.
    static constexpr std::uint32_t MAGIC = 0x46546C67;
    static constexpr std::uint32_t JSON_CHUNK_TYPE = 0x4E4F534A;
    static constexpr std::uint32_t BINARY_CHUNK_TYPE = 0x004E4942;

    namespace GLValue {
        enum {
            // filters
            NEAREST = 9728,
            LINEAR = 9729,
            NEAREST_MIPMAP_NEAREST = 9984,
            LINEAR_MIPMAP_NEAREST = 9985,
            NEAREST_MIPMAP_LINEAR = 9986,
            LINEAR_MIPMAP_LINEAR = 9987,

            // wrap
            CLAMP_TO_EDGE = 33071,
            MIRRORED_REPEAT = 33648,
            REPEAT = 10497,

            // buffer target
            ARRAY_BUFFER = 34962,
            ELEMENT_ARRAY_BUFFER = 34963,

            // types
            BYTE = 5120,
            UNSIGNED_BYTE = 5121,
            SHORT = 5122,
            UNSIGNED_SHORT = 5123,
            UNSIGNED_INT = 5125,
            FLOAT = 5126,
        };
    }

    /// Class that holds the GLB meta data.
    class GlbData {
    public:
        std::string path;
        std::uint32_t chunkLength;
        std::uint32_t offset;
        GlbData() = default;
        ~GlbData() = default;
        // Support moving
        GlbData(GlbData&&) = default;
        GlbData& operator=(GlbData&&) = default;
        // Don't support copying
        GlbData(const GlbData&) = delete;
        GlbData& operator=(const GlbData&) = delete;
    };

    /// The root glTF object.
    /// Use this class to load a file.
    class Gltf final {
    public:
        Gltf() {}
        /// Creates a Gltf object and loads a file.
        /// @param path Path to the file to load.
        explicit Gltf(const char* path) {
            load(path);
        }
        ~Gltf() = default;
        // support moving
        Gltf(Gltf&&) = default;
        Gltf& operator=(Gltf&&) = default;

        // don't support copying
        Gltf(const Gltf&) = delete;
        Gltf& operator=(const Gltf&) = delete;

        /// Loads a glTF 2.0 file.
        /// @param[in] path Path to the file to load.
        /// @return True if json file was loaded successful; false otherwise.
        bool load(const char* path) noexcept;

        /// Returns true if a glTF 2.0 file was loaded successfully.
        operator bool() const noexcept {
            return m_doc != nullptr;
        }

        /// Returns the index of the default scene.
        bool defaultScene(size_t& index) {
            if (m_doc) {
                const auto& it = m_doc->FindMember("scene");
                if (it != m_doc->MemberEnd()) {
                    if (it->value.IsNumber()) {
                        index = it->value.GetUint();
                        return true;
                    }
                }
            }
            return false;
        }

        /// Returns the default scene. This may not be the scene at index zero.
        Scene defaultScene() const noexcept;

        Scene scene(size_t index) const noexcept;
        /// Returns the number of scenes.
        size_t sceneCount() const noexcept {
            return count("scenes");
        }

        Node node(size_t index) const noexcept;
        size_t nodeCount() const noexcept {
            return count("nodes");
        }

        Mesh mesh(size_t index) const noexcept;
        size_t meshCount() const noexcept {
            return count("meshes");
        }

        Camera camera(size_t index) const noexcept;
        size_t cameraCount() const noexcept {
            return count("cameras");
        }

        Accessor accessor(size_t index) const noexcept;
        size_t accessorCount() const noexcept {
            return count("accessors");
        }

        Buffer buffer(size_t index) const noexcept;
        size_t bufferCount() const noexcept {
            return count("buffers");
        }

        BufferView bufferView(size_t index) const noexcept;
        size_t bufferViewCount() const noexcept {
            return count("bufferViews");
        }

        Animation animation(size_t index) const noexcept;
        size_t animationCount() const noexcept {
            return count("animations");
        }

        Image image(size_t index) const noexcept;
        size_t imageCount() const noexcept {
            return count("images");
        }

        Texture texture(size_t index) const noexcept;
        size_t textureCount() const noexcept {
            return count("textures");
        }

        Sampler sampler(size_t index) const noexcept;
        size_t samplerCount() const noexcept {
            return count("samplers");
        }

        Material material(size_t index) const noexcept;
        size_t materialCount() const noexcept {
            return count("materials");
        }

        Skin skin(size_t index) const noexcept;
        size_t skinCount() const noexcept {
            return count("skins");
        }

        Asset asset() const noexcept;

        /// Finds a node by name.
        Node findNode(const char* name);
        Mesh findMesh(const char* name);
        Skin findSkin(const char* name);
        Material findMaterial(const char* name);

        /// Returns a pointer to the json document. May be null.
        const JsonDocument* doc() const noexcept {
            return m_doc.get();
        }

        // TODO friend operator==
        friend class Buffer;

    private:

        size_t count(const char* key) const noexcept {
            if (m_doc) {
                const auto& it = m_doc->FindMember(key);
                if (it != m_doc->MemberEnd()) {
                    return it->value.Size();
                }
            }
            return 0;
        }

        template<typename T>
        T findByName(const char* key, const char* name) {
            const auto& it = m_doc->FindMember(key);
            if (name != nullptr && it != m_doc->MemberEnd() && it->value.IsArray()) {
                const auto size = it->value.Size();
                for (size_t i = 0; i < size; ++i) {
                    const auto& v = it->value[i];
                    const auto& nameIt = v.FindMember("name");
                    if (nameIt != v.MemberEnd() && nameIt->value.IsString()) {
                        if (strcmp(name, nameIt->value.GetString()) == 0) {
                            return T(this, &v);
                        }
                    }
                }
            }
            return T();
        }

        bool loadGlbMetaData(const char* path);
        template<typename T>
        bool loadGlbData(std::vector<T>& data) const noexcept;

        void clear() noexcept {
            m_doc.reset(nullptr);
            m_glb.reset(nullptr);
            m_baseDir.clear();
        }

        std::unique_ptr<JsonDocument> m_doc;
        std::unique_ptr<GlbData> m_glb;
        std::string m_baseDir;
    };

    // functions

    static inline char lowercase(char ch) {
        return ch >= 'A' && ch <= 'Z' ? ch | 0x20 : ch;
    }

    /// Returns true if subject starts with prefix.
    inline bool startsWith(const char* subject, const char* prefix) {
        if (subject == prefix) {
            return true;
        }
        if (subject == nullptr || prefix == nullptr) {
            return false;
        }
        return strncmp(prefix, subject, strlen(prefix)) == 0;
    }

    inline std::string baseDir(const char* path) {
        // This is copied from the code I wrote in gameplay3d
        if (path == nullptr || strlen(path) == 0) {
            return "";
        }
#ifdef _WIN32
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
        std::string dirname;
        size_t driveLength = strlen(drive);
        if (driveLength > 0) {
            dirname.reserve(driveLength + strlen(dir));
            dirname.append(drive);
            dirname.append(dir);
        }
        else {
            dirname.assign(dir);
        }
        std::replace(dirname.begin(), dirname.end(), '\\', '/');
        return dirname;
#else
        // dirname() modifies the input string so create a temp string
        std::string dirname;
        char* tempPath = new char[strlen(path) + 1];
        strcpy(tempPath, path);
        char* dir = ::dirname(tempPath);
        if (dir && strlen(dir) > 0) {
            dirname.assign(dir);
            // dirname() strips off the trailing '/' so add it back to be consistent with Windows
            dirname.append("/");
        }
        delete[] tempPath;
        return dirname;
#endif
    }

    template<typename T>
    static bool findNumber(const JsonValue* ptr, const char* key, T& value) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsNumber()) {
                value = it->value.Get<T>();
                return true;
            }
        }
        return false;
    }

    template<typename T>
    static bool findNumber(const JsonValue* ptr, const char* key, size_t index, T& value) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsNumber()) {
                    value = v.Get<T>();
                    return true;
                }
            }
        }
        return false;
    }

    template<typename T>
    static T findNumberOrDefault(const JsonValue* ptr, const char* key, T defaultValue) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsNumber()) {
                return it->value.Get<T>();
            }
        }
        return defaultValue;
    }

    template<typename T>
    static T findNumberOrDefault(const JsonValue* ptr, const char* key, size_t index, T defaultValue) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsNumber()) {
                    return v.Get<T>();
                }
            }
        }
        return defaultValue;
    }

    template<typename T>
    static bool findNumberInMap(const JsonValue* ptr, const char* key1, const char* key2, T& value) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key1);
            if (it != ptr->MemberEnd() && it->value.IsObject()) {
                auto it2 = it->value.FindMember(key2);
                if (it2 != it->value.MemberEnd() && it2->value.IsNumber()) {
                    value = it2->value.Get<T>();
                    return true;
                }
            }
        }
        return false;
    }

    template<typename T>
    static std::vector<T> getVector(const JsonValue* ptr, const char* key) {
        std::vector<T> vec;
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsArray()) {
                const size_t size = it->value.Size();
                for (size_t index = 0; index < size; ++index) {
                    const auto& v = it->value[index];
                    if (v.IsNumber()) {
                        vec.push_back(v.Get<size_t>());
                    }
                }
            }
        }
        return vec;
    }

    template<typename T>
    static T findObject(const Gltf* gltf, const JsonValue* ptr, const char* key) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsObject()) {
                return T(gltf, &it->value);
            }
        }
        return T();
    }

    template<typename T>
    static T findObject(const Gltf* gltf, const JsonValue* ptr, const char* key, size_t index) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsObject()) {
                    return T(gltf, &v);
                }
            }
        }
        return T();
    }

    template<typename T>
    static T findGltfObject(const Gltf* gltf, const char* key, size_t index) {
        const auto doc = gltf->doc();
        if (doc) {
            const auto it = doc->FindMember(key);
            if (it != doc->MemberEnd()) {
                const auto& values = it->value;
                if (values.IsArray() && index < values.Size()) {
                    return T(gltf, &values[index]);
                }
            }
        }
        return T();
    }

    static std::vector<const char*> getKeys(const JsonValue* ptr, const char* key) {
        std::vector<const char*> vec;
        if (ptr != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsObject()) {
                auto& value = it->value;
                const size_t size = value.MemberCount();
                vec.reserve(size);
                for (auto member = value.MemberBegin(); member != value.MemberEnd(); ++member) {
                    vec.push_back(member->name.GetString());
                }
            }
        }
        return vec;
    }

    /// Copys numbers from a json element
    /// @return True if the numbers were copied.
    template<typename T>
    static bool copyNumbers(const JsonValue* ptr, const char* key, size_t count, T* m) noexcept {
        if (ptr != nullptr && m != nullptr) {
            auto it = ptr->FindMember(key);
            if (it != ptr->MemberEnd() && it->value.IsArray() && count <= it->value.Size()) {
                for (auto& v : it->value.GetArray()) {
                    *m++ = v.Get<T>();
                }
                return true;
            }
        }
        return false;
    }

    template<typename T>
    static bool readBase64(const char* text, size_t byteLength, std::vector<T>&data) {
        std::istringstream in(text);
        data.clear();
        data.reserve(byteLength);
        lib64::decoder decoder;
        lib64::base64_init_decodestate(&decoder._state);
        std::array<char, 1024> code;
        std::array<char, 1024> plaintext;
        int codelength;
        do {
            in.read(code.data(), code.size());
            codelength = static_cast<int>(in.gcount());
            int plainlength = decoder.decode(code.data(), codelength, plaintext.data());
            data.insert(data.end(), plaintext.begin(), plaintext.begin() + plainlength);
        } while (in.good() && codelength > 0);
        base64_init_decodestate(&decoder._state);
        return true;
    }

    template<typename T>
    static bool readBinaryFile(const char* path, size_t byteLength, std::vector<T>& data) {
        unique_file_ptr file(fopen(path, "rb"));
        FILE* fp = file.get();
        if (!fp) {
            return false;
        }
        data.resize(byteLength);
        if (fread(data.data(), 1, byteLength, fp) == byteLength) {
            return true;
        }
        return false;
    }

    /// Base class for GLTF objects.
    class Object {
    public:
        Object() : m_gltf(nullptr), m_json(nullptr) {}
        Object(const Gltf* gltf, const JsonValue* ptr) : m_gltf(gltf), m_json(ptr) {}
        virtual ~Object() {}
        bool isNull() const noexcept { return m_gltf == nullptr; }
        /// Returns true if this Object is not null. (Meaning it is usable)
        operator bool() const noexcept { return m_gltf != nullptr; }

        /// Returns a string property from this GLTF object.
        /// The string returned by the method will become invalid when the json document is unloaded
        /// make a copy if you need it long term.
        /// @param key The key of the property.
        /// @return The string or null if not found.
        const char* str(const char* key) const noexcept {
            if (m_json != nullptr && key != nullptr) {
                auto it = m_json->FindMember(key);
                if (it != m_json->MemberEnd() && it->value.IsString()) {
                    return it->value.GetString();
                }
            }
            return nullptr;
        }
        friend bool operator==(const Object& lhs, const Object& rhs);
    protected:
        /// Returns the size of an array or the number of members in an object.
        size_t count(const char* key) const noexcept {
            if (m_json != nullptr) {
                auto it = m_json->FindMember(key);
                if (it != m_json->MemberEnd()) {
                    if (it->value.IsArray()) {
                        return it->value.Size();
                    }
                    else if (it->value.MemberCount()) {
                        return it->value.MemberCount();
                    }
                }
            }
            return 0;
        }
        bool copyFloats(const char* key, size_t count, float* m) const noexcept {
            // TODO remove and replace with tempalte?
            if (m_json != nullptr && m != nullptr) {
                auto it = m_json->FindMember(key);
                if (it != m_json->MemberEnd() && it->value.IsArray() && count <= it->value.Size()) {
                    for (auto& v : it->value.GetArray()) {
                        *m++ = v.GetFloat();
                    }
                    return true;
                }
            }
            return false;
        }

        float findFloat(const char* key, float defaultValue = 0.0f) const noexcept {
            if (m_json != nullptr) {
                auto it = m_json->FindMember(key);
                if (it != m_json->MemberEnd() && it->value.IsNumber()) {
                    return it->value.GetFloat();
                }
            }
            return defaultValue;
        }

        bool findBool(const char* key, bool defaultValue = false) const noexcept {
            if (m_json != nullptr) {
                auto it = m_json->FindMember(key);
                if (it != m_json->MemberEnd() && it->value.IsBool()) {
                    return it->value.GetBool();
                }
            }
            return defaultValue;
        }

        const Gltf* m_gltf;
        const JsonValue* m_json;
    };

    inline bool operator==(const Object& lhs, const Object& rhs) {
        return lhs.m_gltf == rhs.m_gltf && lhs.m_json == rhs.m_json;
    }

    inline bool operator!=(const Object& lhs, const Object& rhs) {
        return !(lhs == rhs);
    }

    /// A gltf object that has a string name.
    class Named : public Object {
    public:
        Named() {}
        Named(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}
        const char* name() const noexcept {
            return str("name");
        }
    };

    /// Orthographic camera
    class Orthographic : public Object {
    public:
        Orthographic() {}
        Orthographic(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        float xmag() const noexcept {
            return findFloat("xmag");
        }
        float ymag() const noexcept {
            return findFloat("ymag");
        }
        float zfar() const noexcept {
            return findFloat("zfar");
        }
        float znear() const noexcept {
            return findFloat("znear");
        }
    };

    /// Perspective camera
    class Perspective : public Object {
    public:
        Perspective() {}
        Perspective(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        float aspectRatio() const noexcept {
            return findFloat("aspectRatio", DEFAULT_ASPECT_RATIO);
        }

        float yfov() const noexcept {
            return findFloat("yfov");
        }

        float zfar() const noexcept {
            return findFloat("zfar");
        }

        float znear() const noexcept {
            return findFloat("znear");
        }
    };

    class Camera : public Named {
    public:
        Camera() {}
        Camera(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        enum class Type {
            ORTHOGRAPHIC,
            PERSPECTIVE
        };

        Type type() const noexcept {
            if (m_json != nullptr) {
                auto it = m_json->FindMember("type");
                if (it != m_json->MemberEnd() && it->value.IsString()) {
                    const char* str = it->value.GetString();
                    return strcmp(str, "orthographic") == 0 ? Type::ORTHOGRAPHIC : Type::PERSPECTIVE;
                }
            }
            return Type::PERSPECTIVE;
        }

        Orthographic orthographic() const noexcept {
            return findObject<Orthographic>(m_gltf, m_json, "orthographic");
        }

        Perspective perspective() const noexcept {
            return findObject<Perspective>(m_gltf, m_json, "perspective");
        }
    };

    class Node : public Named {
    public:
        Node() {};
        Node(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        Node operator[](size_t index) const noexcept {
            return child(index);
        }
        Node child(size_t index) const noexcept {
            size_t value;
            if (findNumber(m_json, "children", index, value)) {
                return m_gltf->node(value);
            }
            return Node();
        }
        size_t childCount() const noexcept {
            return count("children");
        }
        std::vector<size_t> children() const noexcept {
            return getVector<size_t>(m_json, "children");
        }
        std::vector<size_t> nodes() const noexcept {
            return children();
        }

        bool matrix(float* m) const noexcept {
            return copyFloats("matrix", 16, m);
        }
        bool translation(float* m) const noexcept {
            return copyFloats("translation", 3, m);
        }
        bool rotation(float* m) const noexcept {
            return copyFloats("rotation", 4, m);
        }
        bool scale(float* m) const noexcept {
            return copyFloats("scale", 3, m);
        }

        bool weight(float* p, size_t count) const noexcept {
            return copyFloats("weights", count, p);
        }
        float weight(size_t index) const noexcept {
            return findNumberOrDefault(m_json, "weights", index, 0.0f);
        }
        size_t weightCount() const noexcept {
            return count("weights");
        }

        Mesh mesh() const noexcept;
        bool mesh(size_t& index) const noexcept {
            return findNumber(m_json, "mesh", index);
        }

        Camera camera() const noexcept;
        bool camera(size_t& index) const noexcept {
            return findNumber(m_json, "camera", index);
        }

        Skin skin() const noexcept;
        bool skin(size_t& index) const noexcept {
            return findNumber(m_json, "skin", index);
        }
    };

    class Scene : public Named {
    public:
        Scene() {};
        Scene(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        Node operator[](size_t index) const noexcept {
            return node(index);
        }

        Node node(size_t index) const noexcept {
            size_t value;
            if (findNumber(m_json, "nodes", index, value)) {
                return m_gltf->node(value);
            }
            return Node();
        }

        size_t nodeCount() const noexcept {
            return count("nodes");
        }

        std::vector<size_t> nodes() const noexcept {
            return getVector<size_t>(m_json, "nodes");
        }
    };

    class Buffer : public Named {
    public:
        Buffer() {}
        Buffer(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        const char* uri() const noexcept {
            return str("uri");
        }
        size_t byteLength() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "byteLength", 0);
        }
        template<typename T>
        bool load(std::vector<T>& data) const noexcept;
        // TODO method to get values regardless of location
    };

    class BufferView : public Named {
    public:
        BufferView() {}
        BufferView(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        enum Target {
            ARRAY_BUFFER = 34962,
            ELEMENT_ARRAY_BUFFER = 34963
        };

        Buffer buffer() const noexcept {
            size_t num;
            if (findNumber(m_json, "buffer", num)) {
                return m_gltf->buffer(num);
            }
            return Buffer();
        }
        bool buffer(size_t& index) const noexcept {
            return findNumber(m_json, "buffer", index);
        }
        size_t byteOffset() const noexcept {
            return findNumberOrDefault(m_json, "byteOffset", 0);
        }
        size_t byteLength() const noexcept {
            return findNumberOrDefault(m_json, "byteLength", 1);
        }
        size_t byteStride() const noexcept {
            return findNumberOrDefault(m_json, "byteStride", 4);
        }
        Target target() const noexcept {
            int num;
            if (findNumber<int>(m_json, "target", num)) {
                switch (num) {
                case GLValue::ELEMENT_ARRAY_BUFFER:
                    return Target::ELEMENT_ARRAY_BUFFER;;
                case GLValue::ARRAY_BUFFER:
                    return Target::ARRAY_BUFFER;
                }
            }
            return Target::ARRAY_BUFFER;
        }
        bool hasTarget() const noexcept {
            int num;
            return findNumber<int>(m_json, "target", num);
        }
    };

    class SparseValues : public Object {
    public:
        SparseValues() {}
        SparseValues(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        BufferView bufferView() const noexcept {
            size_t index;
            if (findNumber(m_json, "bufferView", index)) {
                return m_gltf->bufferView(index);
            }
            return BufferView();
        }

        size_t byteOffset() const noexcept {
            return findNumberOrDefault(m_json, "byteOffset", 0);
        }
    };

    class SparseIndices : public Object {
    public:
        SparseIndices() {}
        SparseIndices(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        enum class ComponentType {
            UNSIGNED_BYTE = 5121,
            UNSIGNED_SHORT = 5123,
            UNSIGNED_INT = 5125
        };

        BufferView bufferView() const noexcept {
            size_t index;
            if (findNumber(m_json, "bufferView", index)) {
                return m_gltf->bufferView(index);
            }
            return BufferView();
        }

        size_t byteOffset() const noexcept {
            return findNumberOrDefault(m_json, "byteOffset", 0);
        }

        ComponentType componentType() const noexcept {
            int num;
            if (findNumber<int>(m_json, "componentType", num)) {
                switch (num) {
                case GLValue::UNSIGNED_BYTE: return ComponentType::UNSIGNED_BYTE;
                case GLValue::UNSIGNED_SHORT: return ComponentType::UNSIGNED_SHORT;
                case GLValue::UNSIGNED_INT: return ComponentType::UNSIGNED_INT;
                }
            }
            return ComponentType::UNSIGNED_INT;
        }
    };

    class Sparse : public Object {
    public:
        Sparse() {}
        Sparse(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        size_t count() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "count", 1);
        }

        SparseIndices indices() const noexcept {
            return findObject<SparseIndices>(m_gltf, m_json, "indices");
        }

        SparseValues values() const noexcept {
            return findObject<SparseValues>(m_gltf, m_json, "values");
        }
    };

    class Accessor : public Named {
    public:
        Accessor() {}
        Accessor(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        enum class Type {
            SCALAR,
            VEC2,
            VEC3,
            VEC4,
            MAT2,
            MAT3,
            MAT4
        };

        enum class ComponentType {
            BYTE = 5120,
            UNSIGNED_BYTE = 5121,
            SHORT = 5122,
            UNSIGNED_SHORT = 5123,
            UNSIGNED_INT = 5125,
            FLOAT = 5126
        };

        Type type() const noexcept {
            const char* s = str("type");
            if (s != nullptr) {
                switch (*s) {
                case 'S':
                    return Accessor::Type::SCALAR;
                case 'V':
                    switch (s[3]) {
                    case '2': return Accessor::Type::VEC2;
                    case '3': return Accessor::Type::VEC3;
                    case '4': return Accessor::Type::VEC4;
                    }
                    break;
                case 'M':
                    switch (s[3]) {
                    case '2': return Accessor::Type::VEC2;
                    case '3': return Accessor::Type::VEC3;
                    case '4': return Accessor::Type::VEC4;
                    }
                }
            }
            return Accessor::Type::SCALAR;
        }
        BufferView bufferView() const noexcept {
            size_t num;
            if (findNumber<size_t>(m_json, "bufferView", num)) {
                return m_gltf->bufferView(num);
            }
            return BufferView();
        }
        bool bufferView(size_t& index) const noexcept {
            return findNumber<size_t>(m_json, "bufferView", index);
        }
        size_t byteOffset() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "byteOffset", 0);
        }
        ComponentType componentType() const noexcept {
            int num;
            if (findNumber<int>(m_json, "componentType", num)) {
                switch (num) {
                case GLValue::BYTE: return ComponentType::BYTE;
                case GLValue::UNSIGNED_BYTE: return ComponentType::UNSIGNED_BYTE;
                case GLValue::SHORT: return ComponentType::SHORT;
                case GLValue::UNSIGNED_SHORT: return ComponentType::UNSIGNED_SHORT;
                case GLValue::UNSIGNED_INT: return ComponentType::UNSIGNED_INT;
                case GLValue::FLOAT: return ComponentType::FLOAT;
                }
            }
            return ComponentType::FLOAT;
        }
        bool normalized() const noexcept {
            return findBool("normalized");
        }
        size_t count() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "count", 0);
        }
        bool max(float* p, size_t count) const noexcept {
            return copyFloats("max", count, p);
        }
        float max(size_t index) const noexcept {
            return findNumberOrDefault(m_json, "max", index, 0.0f);
        }
        size_t maxCount() const noexcept {
            return Object::count("max");
        }
        bool min(float* p, size_t count) const noexcept {
            return copyFloats("max", count, p);
        }
        float min(size_t index) const noexcept {
            return findNumberOrDefault(m_json, "min", index, 0.0f);
        }
        size_t minCount() const noexcept {
            return Object::count("min");
        }
        Sparse sparse() const noexcept {
            return findObject<Sparse>(m_gltf, m_json, "sparse");
        }
    };

    class Asset : public Object {
        friend Gltf;
    public:
        Asset() {};
        const char* copyright() const noexcept {
            return str("copyright");
        }
        const char* generator() const noexcept {
            return str("generator");
        }
        const char* version() const noexcept {
            return str("version");
        }
        const char* minVersion() const noexcept {
            return str("minVersion");
        }
    private:
        Asset(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}
    };

    class Image : public Named {
    public:
        Image() {}
        Image(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        const char* uri() const noexcept {
            return str("uri");
        }
        const char* mimeType() const noexcept {
            return str("mimeType");
        }
        BufferView bufferView() const noexcept {
            size_t num;
            if (findNumber(m_json, "bufferView", num)) {
                return m_gltf->bufferView(num);
            }
            return BufferView();
        }
        bool bufferView(size_t& index) const noexcept {
            return findNumber(m_json, "bufferView", index);
        }
    };

    class Sampler : public Named {
    public:
        Sampler() {}
        Sampler(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        enum class Wrap {
            REPEAT = 10497,
            CLAMP_TO_EDGE = 33071,
            MIRRORED_REPEAT = 33648
        };

        enum class MinFilter {
            NEAREST = 9728,
            LINEAR = 9729,
            NEAREST_MIPMAP_NEAREST = 9984,
            LINEAR_MIPMAP_NEAREST = 9985,
            NEAREST_MIPMAP_LINEAR = 9986,
            LINEAR_MIPMAP_LINEAR = 9987
        };

        enum class MagFilter {
            NEAREST = 9728,
            LINEAR = 9729
        };

        MagFilter magFilter() const noexcept {
            int num;
            if (findNumber(m_json, "magFilter", num)) {
                if (num == GLValue::NEAREST) {
                    return MagFilter::NEAREST;
                }
            }
            return MagFilter::LINEAR;
        }
        MinFilter minFilter() const noexcept {
            int num;
            if (findNumber(m_json, "minFilter", num)) {
                switch (num) {
                case GLValue::NEAREST: return MinFilter::NEAREST;
                case GLValue::LINEAR: return MinFilter::LINEAR;
                case GLValue::NEAREST_MIPMAP_NEAREST: return MinFilter::NEAREST_MIPMAP_NEAREST;
                case GLValue::LINEAR_MIPMAP_NEAREST: return MinFilter::LINEAR_MIPMAP_NEAREST;
                case GLValue::NEAREST_MIPMAP_LINEAR: return MinFilter::NEAREST_MIPMAP_LINEAR;
                case GLValue::LINEAR_MIPMAP_LINEAR: return MinFilter::LINEAR_MIPMAP_LINEAR;
                }
            }
            return MinFilter::LINEAR_MIPMAP_LINEAR;
        }
        Wrap wrapS() const noexcept {
            int num;
            if (findNumber(m_json, "wrapS", num)) {
                switch (num) {
                case GLValue::CLAMP_TO_EDGE: return Wrap::CLAMP_TO_EDGE;
                case GLValue::MIRRORED_REPEAT: return Wrap::MIRRORED_REPEAT;
                case GLValue::REPEAT: return Wrap::REPEAT;
                }
            }
            return Wrap::REPEAT;
        }
        Wrap wrapT() const noexcept {
            int num;
            if (findNumber(m_json, "wrapT", num)) {
                switch (num) {
                case GLValue::CLAMP_TO_EDGE: return Wrap::CLAMP_TO_EDGE;
                case GLValue::MIRRORED_REPEAT: return Wrap::MIRRORED_REPEAT;
                case GLValue::REPEAT: return Wrap::REPEAT;
                }
            }
            return Wrap::REPEAT;
        }
    };

    class Texture : public Named {
    public:
        Texture() {}
        Texture(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        /// Returns the Image used by this texture. This method name is more obvious.
        Image image() const noexcept {
            size_t num;
            if (findNumber(m_json, "source", num)) {
                return m_gltf->image(num);
            }
            return Image();
        }

        /// Returns the Image used by this texture. Same as image().
        Image source() const noexcept {
            return image();
        }

        /// Gets the index of the image used by this texture.
        /// @param[out] index The variable to copy the index to.
        /// @return True if the source index of found.
        bool source(size_t& index) const noexcept {
            return findNumber(m_json, "source", index);
        }
        Sampler sampler() const noexcept {
            size_t num;
            if (findNumber(m_json, "sampler", num)) {
                return m_gltf->sampler(num);
            }
            return Sampler();
        }

        bool sampler(size_t& index) const noexcept {
            return findNumber(m_json, "sampler", index);
        }
    };

    class TextureInfo : public Object {
    public:
        TextureInfo() {}
        TextureInfo(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        /// Returns the texture instead of having to use index().
        Texture texture() const noexcept {
            if (m_gltf != nullptr) {
                return m_gltf->texture(index());
            }
            return Texture();
        }

        size_t texCoord() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "texCoord", 0);
        }

        /// The index of the texture (when using Gltf::texture(size_t))
        size_t index() const noexcept {
            return findNumberOrDefault<size_t>(m_json, "index", 0);
        }

        bool index(size_t& index) const noexcept {
            return findNumber(m_json, "index", index);
        }
    };

    class Skin : public Named {
    public:
        Skin() {}
        Skin(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        Accessor inverseBindMatrices() const noexcept {
            size_t num;
            if (findNumber<size_t>(m_json, "inverseBindMatrices", num)) {
                return m_gltf->accessor(num);
            }
            return Accessor();
        }
        bool inverseBindMatrices(size_t& index) const noexcept {
            return findNumber<size_t>(m_json, "inverseBindMatrices", index);
        }
        Node skeleton() const noexcept {
            size_t num;
            if (findNumber<size_t>(m_json, "skeleton", num)) {
                return m_gltf->node(num);
            }
            // Spec says: "When undefined, joints transforms resolve to scene root."
            // I'll leave it up to the caller to find the root node.
            return Node();
        }
        bool skeleton(size_t& index) const noexcept {
            return findNumber<size_t>(m_json, "skeleton", index);
        }
        bool joints(size_t* p, size_t count) const noexcept {
            return copyNumbers<size_t>(m_json, "joints", count, p);
        }
        size_t joint(size_t index) const noexcept {
            return findNumberOrDefault<size_t>(m_json, "joints", index, 0);
        }
        size_t jointCount() const noexcept {
            return count("joints");
        }
        std::vector<size_t> joints() const noexcept {
            return getVector<size_t>(m_json, "joints");
        }
    };

    class NormalTextureInfo : public TextureInfo {
    public:
        NormalTextureInfo() {}
        NormalTextureInfo(const Gltf* gltf, const JsonValue* ptr) : TextureInfo(gltf, ptr) {}

        float scale() const noexcept {
            return findFloat("scale", 1.0f);
        }
    };

    class PbrMetallicRoughness : public Object {
    public:
        PbrMetallicRoughness() {}
        PbrMetallicRoughness(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        /// Copies 4 floats to the given pointer.
        bool baseColorFactor(float* color) const noexcept {
            if (color == nullptr) {
                return false;
            }
            if (!copyFloats("baseColorFactor", 4, color)) {
                // failed read baseColorFactor so set to 1,1,1,1
                std::fill(color, color + 4, 1.0f);
            }
            return true;
        }
        TextureInfo baseColorTexture() const noexcept {
            return findObject<TextureInfo>(m_gltf, m_json, "baseColorTexture");
        }
        float metallicFactor() const noexcept {
            return findFloat("metallicFactor", 1.0f);
        }
        float roughnessFactor() const noexcept {
            return findFloat("roughnessFactor", 1.0f);
        }
        TextureInfo metallicRoughnessTexture() const noexcept {
            return findObject<TextureInfo>(m_gltf, m_json, "metallicRoughnessTexture");
        }
    };

    class OcclusionTextureInfo : public TextureInfo {
    public:
        OcclusionTextureInfo() {}
        OcclusionTextureInfo(const Gltf* gltf, const JsonValue* ptr) : TextureInfo(gltf, ptr) {}

        float strength() const noexcept {
            return findFloat("strength", 1.0f);
        }
    };

    class Material : public Named {
    public:
        Material() {}
        Material(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        enum class AlphaMode {
            OPAQUE,
            MASK,
            BLEND
        };

        PbrMetallicRoughness pbrMetallicRoughness() const noexcept {
            return findObject<PbrMetallicRoughness>(m_gltf, m_json, "pbrMetallicRoughness");
        }
        NormalTextureInfo normalTexture() const noexcept {
            return findObject<NormalTextureInfo>(m_gltf, m_json, "normalTexture");
        }
        OcclusionTextureInfo occlusionTexture() const noexcept {
            return findObject<OcclusionTextureInfo>(m_gltf, m_json, "occlusionTexture");
        }
        TextureInfo emissiveTexture() const noexcept {
            return findObject<TextureInfo>(m_gltf, m_json, "emissiveTexture");
        }
        bool emissiveFactor(float* emissive) const noexcept {
            return copyFloats("emissiveFactor", 3, emissive);
        }
        AlphaMode alphaMode() const noexcept {
            const char* s = str("alphaMode");
            if (s != nullptr) {
                switch (*s) {
                case 'M': return AlphaMode::MASK;
                case 'B': return AlphaMode::BLEND;
                }
            }
            return AlphaMode::OPAQUE;
        }
        float alphaCutoff() const noexcept {
            return findFloat("alphaCutoff", 0.5f);
        }
        bool doubleSided() const noexcept {
            return findBool("doubleSided");
        }
    };

    class MorphTarget : public Object {
    public:
        MorphTarget() {}
        MorphTarget(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        size_t position() const noexcept {
            return findNumberOrDefault(m_json, "POSITION", 0);
        }
        size_t normal() const noexcept {
            return findNumberOrDefault(m_json, "NORMAL", 0);
        }
        size_t tangent() const noexcept {
            return findNumberOrDefault(m_json, "TANGENT", 0);
        }
    };

    class Primitive : public Object {
    public:
        Primitive() {}
        Primitive(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        enum class Mode {
            POINTS = 0,
            LINES = 1,
            LINE_LOOP = 2,
            LINE_STRIP = 3,
            TRIANGLES = 4,
            TRIANGLE_STRIP = 5,
            TRIANGLE_FAN = 6
        };

        Mode mode() const noexcept {
            int value = 4;
            findNumber(m_json, "mode", value);
            switch (value) {
            case 0: return Mode::POINTS;
            case 1: return Mode::LINES;
            case 2: return Mode::LINE_LOOP;
            case 3: return Mode::LINE_STRIP;
            case 4: return Mode::TRIANGLES;
            case 5: return Mode::TRIANGLE_STRIP;
            case 6: return Mode::TRIANGLE_FAN;
            default: return Mode::TRIANGLES;
            }
        }

        Accessor attribute(const char* attribute) const noexcept {
            size_t value;
            if (findNumberInMap(m_json, "attributes", attribute, value)) {
                return m_gltf->accessor(value);
            }
            return Accessor();
        }
        /// Returns the list of attributes as a vector of pairs.
        /// Pair.first is the attribute name and pair.second is the Accessor index.
        std::vector<std::pair<const char*, size_t>> attributes() const noexcept {
            std::vector<std::pair<const char*, size_t>> vec;
            if (m_json != nullptr) {
                auto it = m_json->FindMember("attributes");
                if (it != m_json->MemberEnd() && it->value.IsObject()) {
                    vec.reserve(it->value.MemberCount());
                    for (auto member = it->value.MemberBegin(); member != it->value.MemberEnd(); ++member) {
                        vec.emplace_back(member->name.GetString(), member->value.Get<size_t>());
                    }
                }
            }
            return vec;
        }

        /// Returns the number of attributes in this primitive.
        size_t attributeCount() const noexcept {
            return count("attributes");
        }
        /// Returns a list of attribute names from this primitive.
        /// The char pointers in this list will be invalid when the root gltf object is destroyed 
        /// or loads a new file. 
        std::vector<const char*> attributeStrings() const noexcept {
            return getKeys(m_json, "attributes");
        }
        Accessor position() const noexcept {
            return attribute("POSITION");
        }
        Accessor normal() const noexcept {
            return attribute("NORMAL");
        }
        Accessor tangent() const noexcept {
            return attribute("TANGENT");
        }
        Accessor texcoord(size_t index) const noexcept {
            // OpenGL ES 3.0 supports a min of 16 vertex attributes but I'm
            // going to assume there won't be more than 10 of one type. For now.
            assert(index < 10);
            char attr[] = "TEXCOORD_0";
            attr[9] += static_cast<char>(index);
            return attribute(attr);
        }
        Accessor color(size_t index = 0) const noexcept {
            assert(index < 10);
            char attr[] = "COLOR_0";
            attr[6] += static_cast<char>(index);
            return attribute(attr);
        }
        Accessor joints(size_t index = 0) const noexcept {
            assert(index < 10);
            char attr[] = "JOINTS_0";
            attr[7] += static_cast<char>(index);
            return attribute(attr);
        }
        Accessor weights(size_t index = 0) const noexcept {
            assert(index < 10);
            char attr[] = "WEIGHTS_0";
            attr[8] += static_cast<char>(index);
            return attribute(attr);
        }
        Accessor indices() const noexcept {
            size_t index;
            if (findNumber(m_json, "indices", index)) {
                return m_gltf->accessor(index);
            }
            return Accessor();
        }
        bool indices(size_t& index) const noexcept {
            return findNumber(m_json, "indices", index);
        }
        Material material() const noexcept {
            size_t num;
            if (findNumber<size_t>(m_json, "material", num)) {
                return m_gltf->material(num);
            }
            return Material();
        }
        bool material(size_t& index) const noexcept {
            return findNumber<size_t>(m_json, "material", index);
        }
        MorphTarget target(size_t index) const noexcept {
            return findObject<MorphTarget>(m_gltf, m_json, "targets", index);
        }
        size_t targetCount() const noexcept {
            return count("targets");
        }
    };

    class Mesh : public Named {
    public:
        Mesh() {}
        Mesh(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        Primitive primitive(size_t index) const noexcept {
            return findObject<Primitive>(m_gltf, m_json, "primitives", index);
        }

        size_t primitiveCount() const noexcept {
            return count("primitives");
        }

        float weight(size_t index) const noexcept {
            return findNumberOrDefault(m_json, "weights", index, 0.0f);
        }

        bool weights(float* p, size_t count) const noexcept {
            return copyFloats("weights", count, p);
        }

        size_t weightCount() const noexcept {
            return count("weights");
        }
    };

    class AnimationSampler : public Object {
    public:
        AnimationSampler() {}
        AnimationSampler(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

        Accessor input() const noexcept {
            size_t index;
            if (input(index)) {
                return m_gltf->accessor(index);
            }
            return Accessor();
        }
        bool input(size_t& index) const noexcept {
            return findNumber(m_json, "input", index);
        }
        Accessor output() const noexcept {
            size_t index;
            if (output(index)) {
                return m_gltf->accessor(index);
            }
            return Accessor();
        }
        bool output(size_t& index) const noexcept {
            return findNumber(m_json, "output", index);
        }
        Interpolation interpolation() const noexcept {
            const char* s = str("interpolation");
            if (s != nullptr && *s != '\0') {
                switch (s[1]) { // 2nd char
                case 'T': return Interpolation::STEP;
                case 'A': return Interpolation::CATMULLROMSPLINE;
                case 'U': return Interpolation::CUBICSPLINE;
                }
            }
            return Interpolation::LINEAR;
        }
        const char* interpolationStr() const noexcept {
            return str("interpolation");
        }
    };

    class Channel : public Object {
    public:
        Channel() {}
        Channel(const Gltf* gltf, const JsonValue* ptr, const JsonValue* animation) : Object(gltf, ptr), animation(animation) {}

        class Target : public Object {
        public:
            Target() {}
            Target(const Gltf* gltf, const JsonValue* ptr) : Object(gltf, ptr) {}

            Node node() const noexcept {
                size_t index;
                if (node(index)) {
                    return m_gltf->node(index);
                }
                return Node();
            }
            bool node(size_t& index) const noexcept {
                return findNumber(m_json, "node", index);
            }
            TargetPath path() const noexcept {
                const char* s = str("path");
                if (s != nullptr) {
                    switch (*s) {
                    case 'r': return TargetPath::ROTATION;
                    case 's': return TargetPath::SCALE;
                    case 'w': return TargetPath::WEIGHTS;
                    }
                }
                return TargetPath::TRANSLATION;
            }
            const char* pathStr() const noexcept {
                return str("path");
            }
        };

        AnimationSampler sampler() const noexcept {
            size_t index;
            if (sampler(index)) {
                assert(animation);
                return findObject<AnimationSampler>(m_gltf, animation, "samplers", index);
            }
            return AnimationSampler();
        }
        bool sampler(size_t& index) const noexcept {
            return findNumber(m_json, "sampler", index);
        }
        Channel::Target target() const noexcept {
            return findObject<Channel::Target>(m_gltf, m_json, "target");
        }
    private:
        /// Pointer to the parent animation json object
        const JsonValue* animation;
    };

    // TODO move this?
    static Channel findChannel(const Gltf* gltf, const JsonValue* ptr, size_t index) {
        if (ptr != nullptr) {
            auto it = ptr->FindMember("channels");
            if (it != ptr->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsObject()) {
                    return Channel(gltf, &v, ptr);
                }
            }
        }
        return Channel();
    }

    class Animation : public Named {
    public:
        Animation() {}
        Animation(const Gltf* gltf, const JsonValue* ptr) : Named(gltf, ptr) {}

        Channel channel(size_t index) const noexcept {
            return findChannel(m_gltf, m_json, index);
        }
        size_t channelCount() const noexcept {
            return count("channels");
        }
        AnimationSampler sampler(size_t index) const noexcept {
            return findObject<AnimationSampler>(m_gltf, m_json, "samplers", index);
        }
        size_t samplerCount() const noexcept {
            return count("samplers");
        }
    };

    // functions

    template<typename T = size_t>
    T numberOfComponents(Accessor::Type type) noexcept {
        switch (type) {
        case Accessor::Type::SCALAR: return 1;
        case Accessor::Type::VEC2: return 2;
        case Accessor::Type::VEC3: return 3;
        case Accessor::Type::VEC4: return 4;
        case Accessor::Type::MAT2: return 4;
        case Accessor::Type::MAT3: return 9;
        case Accessor::Type::MAT4: return 16;
        default:
            return 0;
        }
    }

    // impl

    inline bool Gltf::load(const char* path) noexcept {
        if (path == nullptr || *path == '\0') {
            return false;
        }
        clear();
        size_t len = strlen(path);
        if (lowercase(path[len - 1]) == 'b') { // .glb
            return loadGlbMetaData(path);
        }
        unique_file_ptr file(fopen(path, "rb"));
        FILE* fp = file.get();
        if (!fp) {
            return false;
        }
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        m_doc.reset(new JsonDocument());
        m_doc->ParseStream(is);
        m_baseDir.assign(baseDir(path));
        return true;
    }

    inline Scene Gltf::defaultScene() const noexcept {
        if (m_doc) {
            const auto& it = m_doc->FindMember("scene");
            if (it != m_doc->MemberEnd()) {
                if (it->value.IsNumber()) {
                    return scene(it->value.GetUint());
                }
            }
        }
        return Scene();
    }

    inline Scene Gltf::scene(size_t index) const noexcept {
        return findGltfObject<Scene>(this, "scenes", index);
    }

    inline Node Gltf::node(size_t index) const noexcept {
        return findGltfObject<Node>(this, "nodes", index);
    }

    inline Mesh Gltf::mesh(size_t index) const noexcept {
        return findGltfObject<Mesh>(this, "meshes", index);
    }

    inline Camera Gltf::camera(size_t index) const noexcept {
        return findGltfObject<Camera>(this, "cameras", index);
    }

    inline Accessor Gltf::accessor(size_t index) const noexcept {
        return findGltfObject<Accessor>(this, "accessors", index);
    }

    inline Buffer Gltf::buffer(size_t index) const noexcept {
        return findGltfObject<Buffer>(this, "buffers", index);
    }

    inline BufferView Gltf::bufferView(size_t index) const noexcept {
        return findGltfObject<BufferView>(this, "bufferViews", index);
    }

    inline Animation Gltf::animation(size_t index) const noexcept {
        return findGltfObject<Animation>(this, "animations", index);
    }

    inline Image Gltf::image(size_t index) const noexcept {
        return findGltfObject<Image>(this, "images", index);
    }

    inline Texture Gltf::texture(size_t index) const noexcept {
        return findGltfObject<Texture>(this, "textures", index);
    }

    inline Sampler Gltf::sampler(size_t index) const noexcept {
        return findGltfObject<Sampler>(this, "samplers", index);
    }

    inline Material Gltf::material(size_t index) const noexcept {
        return findGltfObject<Material>(this, "materials", index);
    }

    inline Skin Gltf::skin(size_t index) const noexcept {
        return findGltfObject<Skin>(this, "skins", index);
    }

    inline Asset Gltf::asset() const noexcept {
        if (m_doc) {
            auto it = m_doc->FindMember("asset");
            if (it != m_doc->MemberEnd()) {
                const auto& v = it->value;
                if (v.IsObject()) {
                    return Asset(this, &v);
                }
            }
        }
        return Asset();
    }

    inline Node Gltf::findNode(const char* name) {
        return findByName<Node>("nodes", name);
    }

    inline Mesh Gltf::findMesh(const char* name) {
        return findByName<Mesh>("meshes", name);
    }

    inline Skin Gltf::findSkin(const char* name) {
        return findByName<Skin>("skins", name);
    }

    inline Material Gltf::findMaterial(const char* name) {
        return findByName<Material>("materials", name);
    }

    inline bool Gltf::loadGlbMetaData(const char* path) {
        unique_file_ptr file(fopen(path, "rb"));
        FILE* fp = file.get();
        if (!fp) {
            return false;
        }
        static constexpr size_t magic = 0;
        static constexpr size_t version = 1;
        static constexpr size_t length = 2;
        static constexpr size_t chunkLength = 3;
        static constexpr size_t chunkType = 4;
        std::array<std::uint32_t, 5> header;
        if (fread(header.data(), sizeof(std::uint32_t), header.size(), fp) == header.size()) {
            if (header[magic] == MAGIC && header[chunkType] == JSON_CHUNK_TYPE) {
                const size_t bufferLength = header[chunkLength];
                std::unique_ptr<char[]> buffer(new char[bufferLength]);
                size_t bytesRead = fread(buffer.get(), 1, bufferLength, fp);
                if (bytesRead != bufferLength) {
                    return false;
                }
                rapidjson::MemoryStream stream(buffer.get(), bufferLength);
                m_doc.reset(new JsonDocument());
                m_doc->ParseStream(stream);

                // attempt to read the binary buffer chunk
                if (std::fseek(fp, sizeof(header) + header[chunkLength], SEEK_SET)) {
                    return true;
                }
                std::uint32_t binaryChunkLength;
                std::uint32_t binaryChunkType;
                if (fread(&binaryChunkLength, sizeof(binaryChunkLength), 1, fp) == 1) {
                    if (fread(&binaryChunkType, sizeof(binaryChunkType), 1, fp) == 1 && binaryChunkType == BINARY_CHUNK_TYPE) {
                        if (!m_glb) {
                            m_glb.reset(new GlbData());
                        }
                        m_glb->path.assign(path);
                        m_glb->offset = static_cast<size_t>(ftell(fp));
                        m_glb->chunkLength = binaryChunkLength;
                    }
                }
                return true;
            }
        }
        return false;
    }

    template<typename T>
    bool Gltf::loadGlbData(std::vector<T>& data) const noexcept {
        static_assert(sizeof(T) == 1, "vector type size must be 1");
        if (m_glb) {
            const auto& chunkLength = m_glb->chunkLength;
            unique_file_ptr file(fopen(m_glb->path.c_str(), "rb"));
            FILE* fp = file.get();
            if (!fp) {
                return false;
            }
            if (std::fseek(fp, m_glb->offset, SEEK_SET)) {
                return false;
            }
            data.resize(chunkLength);
            if (fread(data.data(), 1, chunkLength, fp) == chunkLength) {
                return true;
            }
        }
        return false;
    }

    inline Mesh Node::mesh() const noexcept {
        size_t index;
        if (mesh(index)) {
            return m_gltf->mesh(index);
        }
        return Mesh();
    }

    inline Camera Node::camera() const noexcept {
        size_t index;
        if (camera(index)) {
            return m_gltf->camera(index);
        }
        return Camera();
    }

    inline Skin Node::skin() const noexcept {
        size_t index;
        if (skin(index)) {
            return m_gltf->skin(index);
        }
        return Skin();
    }

    template<typename T>
    bool Buffer::load(std::vector<T>& data) const noexcept {
        static_assert(sizeof(T) == 1, "vector type must be 1 byte (like char or unsigned char)");
        const char* uriStr = uri();
        if (uriStr == nullptr) {
            // GLB
            // TODO verify that this is the first buffer of the buffers array?
            // "glTF Buffer referring to GLB-stored BIN chunk, must have buffer.uri 
            // property undefined, and it must be the first element of buffers array"
            return m_gltf->loadGlbData(data);
        }
        else if (startsWith(uriStr, LAZY_GLTF2_DATA_APP_BASE64)) {
            // base64
            return readBase64(uriStr + sizeof(LAZY_GLTF2_DATA_APP_BASE64) - 1, byteLength(), data);
        }
        else {
            // TODO make sure this is a local file URI
            // external bin
            std::string path = m_gltf->m_baseDir + uriStr;
            return readBinaryFile(path.c_str(), byteLength(), data);
        }
        return false;
    }
}

#endif // LAZY_GLTF2_HPP
