#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>

// TODO decide how big SizeType should be.
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson { typedef ::std::size_t SizeType; }

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "gltf2.hpp"

using namespace rapidjson;
using namespace std;

#define jsonDoc() (*static_cast<Document*>(_doc))

namespace gltf2 {

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

    static inline const Document::GenericValue* genericValue(const void* ptr) {
        return (const Document::GenericValue*)(ptr);
    }

    template<typename T>
    static bool findNumber(const void* ptr, const char* key, T& value) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsNumber()) {
                value = it->value.Get<T>();
                return true;
            }
        }
        return false;
    }

    template<typename T>
    static bool findNumber(const void* ptr, const char* key, size_t index, T& value) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
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
    static T findNumberOrDefault(const void* ptr, const char* key, T defaultValue) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsNumber()) {
                return it->value.Get<T>();
            }
        }
        return defaultValue;
    }

    template<typename T>
    static T findNumberOrDefault(const void* ptr, const char* key, size_t index, T defaultValue) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsNumber()) {
                    return v.Get<T>();
                }
            }
        }
        return defaultValue;
    }

    template<typename T>
    static bool findNumberInMap(const void* ptr, const char* key1, const char* key2, T& value) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key1);
            if (it != p->MemberEnd() && it->value.IsObject()) {
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
    static T findObject(const Gltf* gltf, const void* ptr, const char* key) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsObject()) {
                return T(gltf, &it->value);
            }
        }
        return T();
    }

    template<typename T>
    static T findObject(const Gltf* gltf, const void* ptr, const char* key, size_t index) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
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
        const auto& json = (*static_cast<Document*>(gltf->doc()));
        auto it = json.FindMember(key);
        if (it != json.MemberEnd()) {
            const auto& values = it->value;
            if (values.IsArray() && index < values.Size()) {
                return T(gltf, &values[index]);
            }
        }
        return T();
    }

    static Channel findChannel(const Gltf* gltf, const void* ptr, size_t index) {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember("channels");
            if (it != p->MemberEnd() && it->value.IsArray() && index < it->value.Size()) {
                const auto& v = it->value[index];
                if (v.IsObject()) {
                    return Channel(gltf, &v, ptr);
                }
            }
        }
        return Channel();
    }

    template<typename T>
    static std::vector<T> getVector(const void* ptr, const char* key) {
        std::vector<T> vec;
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray()) {
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

    /// Copys numbers from a json element
    /// @return True if the numbers were copied.
    template<typename T>
    bool copyNumbers(const void* ptr, const char* key, size_t count, T* m) noexcept {
        if (ptr != nullptr && m != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray() && count <= it->value.Size()) {
                for (auto& v : it->value.GetArray()) {
                    *m++ = v.Get<T>();
                }
                return true;
            }
        }
        return false;
    }

    const char* Base::str(const char* key) const noexcept {
        if (ptr != nullptr && key != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsString()) {
                return it->value.GetString();
            }
        }
        return nullptr;
    }

    size_t Base::count(const char* key) const noexcept {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd()) {
                return it->value.Size();
            }
        }
        return 0;
    }

    bool Base::copyFloats(const char* key, size_t count, float* m) const noexcept {
        // TODO remove and replace with tempalte?
        if (ptr != nullptr && m != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsArray() && count <= it->value.Size()) {
                for (auto& v : it->value.GetArray()) {
                    *m++ = v.GetFloat();
                }
                return true;
            }
        }
        return false;
    }

    float Base::findFloat(const char* key, float defaultValue) const noexcept {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsNumber()) {
                return it->value.GetFloat();
            }
        }
        return defaultValue;
    }

    bool Base::findBool(const char* key, bool defaultValue) const noexcept {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember(key);
            if (it != p->MemberEnd() && it->value.IsBool()) {
                return it->value.GetBool();
            }
        }
        return defaultValue;
    }

    const char* Named::name() const noexcept {
        return str("name");
    }

    Gltf::Gltf() : _doc(nullptr) {
    }

    Gltf::Gltf(const char* path) : _doc(nullptr) {
        load(path);
    }

    Gltf::~Gltf() {
        if (_doc != nullptr) {
            delete _doc;
            //auto* d = static_cast<Document*>(_doc);
            //delete d;
        }
    }

    bool Gltf::load(const char* path) noexcept {
        FILE* fp;
        auto err = fopen_s(&fp, path, "r");
        if (err) {
            cout << "error reading file" << endl;
            return false;
        }
        // TODO read first 4 bytes to determine if this is a GLB file

        if (_doc != nullptr) {
            delete _doc;
        }

        char readBuffer[65536];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        _doc = new Document();
        Document& doc = *static_cast<Document*>(_doc);
        doc.ParseStream(is);
        fclose(fp);
        return true;
    }

    bool Gltf::defaultSceneIndex(size_t& index) {
        const auto& json = jsonDoc();
        const auto& it = json.FindMember("scene");
        if (it != json.MemberEnd()) {
            if (it->value.IsNumber()) {
                index = it->value.GetUint();
                return true;
            }
        }
        return false;
    }

    Scene Gltf::scene() const noexcept {
        const auto& json = jsonDoc();
        const auto& it = json.FindMember("scene");
        if (it != json.MemberEnd()) {
            if (it->value.IsNumber()) {
                return scene(it->value.GetUint());
            }
        }
        return Scene();
    }

    Scene Gltf::scene(size_t index) const noexcept {
        return findGltfObject<Scene>(this, "scenes", index);
    }

    size_t Gltf::sceneCount() const noexcept {
        return count("scenes");
    }

    Node Gltf::node(size_t index) const noexcept {
        return findGltfObject<Node>(this, "nodes", index);
    }

    size_t Gltf::nodeCount() const noexcept {
        return count("nodes");
    }

    Mesh Gltf::mesh(size_t index) const noexcept {
        return findGltfObject<Mesh>(this, "meshes", index);
    }

    size_t Gltf::meshCount() const noexcept {
        return count("meshes");
    }

    Camera Gltf::camera(size_t index) const noexcept {
        return findGltfObject<Camera>(this, "cameras", index);
    }

    size_t Gltf::cameraCount() const noexcept {
        return count("cameras");
    }

    Accessor Gltf::accessor(size_t index) const noexcept {
        return findGltfObject<Accessor>(this, "accessors", index);
    }

    size_t Gltf::accessorCount() const noexcept {
        return count("accessors");
    }

    Buffer Gltf::buffer(size_t index) const noexcept {
        return findGltfObject<Buffer>(this, "buffers", index);
    }

    size_t Gltf::bufferCount() const noexcept {
        return count("buffers");
    }

    BufferView Gltf::bufferView(size_t index) const noexcept {
        return findGltfObject<BufferView>(this, "bufferViews", index);
    }

    size_t Gltf::bufferViewCount() const noexcept {
        return count("bufferViews");
    }

    Animation Gltf::animation(size_t index) const noexcept {
        return findGltfObject<Animation>(this, "animations", index);
    }

    size_t Gltf::animationCount() const noexcept {
        return count("animations");
    }

    Image Gltf::image(size_t index) const noexcept {
        return findGltfObject<Image>(this, "images", index);
    }

    size_t Gltf::imageCount() const noexcept {
        return count("images");
    }

    Texture Gltf::texture(size_t index) const noexcept {
        return findGltfObject<Texture>(this, "textures", index);
    }

    size_t Gltf::textureCount() const noexcept {
        return count("textures");
    }

    Sampler Gltf::sampler(size_t index) const noexcept {
        return findGltfObject<Sampler>(this, "samplers", index);
    }

    size_t Gltf::samplerCount() const noexcept {
        return count("samplers");
    }

    Material Gltf::material(size_t index) const noexcept {
        return findGltfObject<Material>(this, "materials", index);
    }

    size_t Gltf::materialCount() const noexcept {
        return count("materials");
    }

    Skin Gltf::skin(size_t index) const noexcept {
        return findGltfObject<Skin>(this, "skins", index);
    }

    size_t Gltf::skinCount() const noexcept {
        return count("skins");
    }

    Asset Gltf::asset() const noexcept {
        const auto& json = jsonDoc();
        auto it = json.FindMember("asset");
        if (it != json.MemberEnd()) {
            const auto& v = it->value;
            if (v.IsObject()) {
                return Asset(this, &v);
            }
        }
        return Asset();
    }

    void* Gltf::doc() const noexcept {
        return _doc;
    }

    size_t Gltf::count(const char* key) const noexcept {
        const auto& json = jsonDoc();
        const auto& it = json.FindMember(key);
        if (it != json.MemberEnd()) {
            return it->value.Size();
        }
        return 0;
    }

    Node Node::operator[](size_t index) const noexcept {
        return child(index);
    }

    Node Node::child(size_t index) const noexcept {
        size_t value;
        if (findNumber(ptr, "children", index, value)) {
            return gltf->node(value);
        }
        return Node();
    }

    size_t Node::childCount() const noexcept {
        return count("children");
    }

    std::vector<size_t> Node::children() const noexcept {
        return getVector<size_t>(ptr, "children");
    }

    std::vector<size_t> Node::nodes() const noexcept {
        return children();
    }

    bool Node::matrix(float* m) const noexcept {
        return copyFloats("matrix", 16, m);
    }

    bool Node::translation(float* m) const noexcept {
        return copyFloats("translation", 3, m);
    }

    bool Node::rotation(float* m) const noexcept {
        return copyFloats("rotation", 4, m);
    }

    bool Node::scale(float* m) const noexcept {
        return copyFloats("scale", 3, m);
    }

    bool Node::weight(float* p, size_t count) const noexcept {
        return copyFloats("weights", count, p);
    }

    float Node::weight(size_t index) const noexcept {
        return findNumberOrDefault(ptr, "weights", index, 0.0f);
    }

    size_t Node::weightCount() const noexcept {
        return count("weights");
    }

    Mesh Node::mesh() const noexcept {
        size_t index;
        if (findNumber(ptr, "mesh", index)) {
            return gltf->mesh(index);
        }
        return Mesh();
    }

    Camera Node::camera() const noexcept {
        size_t index;
        if (findNumber(ptr, "camera", index)) {
            return gltf->camera(index);
        }
        return Camera();
    }

    Skin Node::skin() const noexcept {
        size_t index;
        if (findNumber(ptr, "skin", index)) {
            return gltf->skin(index);
        }
        return Skin();
    }

    Camera::Type Camera::type() const noexcept {
        if (ptr != nullptr) {
            auto p = genericValue(ptr);
            auto it = p->FindMember("type");
            if (it != p->MemberEnd() && it->value.IsString()) {
                const char* str = it->value.GetString();
                return strcmp(str, "orthographic") == 0 ? Type::ORTHOGRAPHIC : Type::PERSPECTIVE;
            }
        }
        return Type::PERSPECTIVE;
    }

    Orthographic Camera::orthographic() const noexcept {
        return findObject<Orthographic>(gltf, ptr, "orthographic");
    }

    Perspective Camera::perspective() const noexcept {
        return findObject<Perspective>(gltf, ptr, "perspective");
    }

    float Orthographic::xmag() const noexcept {
        return findFloat("xmag");
    }

    float Orthographic::ymag() const noexcept {
        return findFloat("ymag");
    }

    float Orthographic::zfar() const noexcept {
        return findFloat("zfar");
    }

    float Orthographic::znear() const noexcept {
        return findFloat("znear");
    }

    float Perspective::aspectRatio() const noexcept {
        return findFloat("aspectRatio");
    }

    float Perspective::yfov() const noexcept {
        return findFloat("yfov");
    }

    float Perspective::zfar() const noexcept {
        return findFloat("zfar");
    }

    float Perspective::znear() const noexcept {
        return findFloat("znear");
    }

    Primitive Mesh::primitive(size_t index) const noexcept {
        return findObject<Primitive>(gltf, ptr, "primitives", index);
    }

    size_t Mesh::primitiveCount() const noexcept {
        return count("primitives");
    }

    bool Mesh::weight(float* p, size_t count) const noexcept {
        return copyFloats("weights", count, p);
    }

    float Mesh::weight(size_t index) const noexcept {
        return findNumberOrDefault(ptr, "weights", index, 0.0f);
    }

    size_t Mesh::weightCount() const noexcept {
        return count("weights");
    }

    Node Scene::operator[](size_t index) const noexcept {
        return node(index);
    }

    Node Scene::node(size_t index) const noexcept {
        size_t value;
        if (findNumber(ptr, "nodes", index, value)) {
            return gltf->node(value);
        }
        return Node();
    }

    size_t Scene::nodeCount() const noexcept {
        return count("nodes");
    }

    std::vector<size_t> Scene::nodes() const noexcept {
        return getVector<size_t>(ptr, "nodes");
    }

    Primitive::Mode Primitive::mode() const noexcept {
        int value = 4;
        findNumber(ptr, "mode", value);
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

    Accessor Primitive::attribute(const char* attribute) const noexcept {
        size_t value;
        if (findNumberInMap(ptr, "attributes", attribute, value)) {
            return gltf->accessor(value);
        }
        return Accessor();
    }

    Accessor Primitive::position() const noexcept {
        return attribute("POSITION");
    }

    Accessor Primitive::normal() const noexcept {
        return attribute("NORMAL");
    }

    Accessor Primitive::tangent() const noexcept {
        return attribute("TANGENT");
    }

    Accessor Primitive::texcoord(size_t index) const noexcept {
        // OpenGL ES 3.0 supports a min of 16 vertex attributes but I'm
        // going to assume there won't be more than 10 of one type.
        assert(index < 10);
        char attr[] = "TEXCOORD_0";
        attr[9] += static_cast<char>(index);
        return attribute(attr);
    }

    Accessor Primitive::color(size_t index) const noexcept {
        assert(index < 10);
        char attr[] = "COLOR_0";
        attr[6] += static_cast<char>(index);
        return attribute(attr);
    }

    Accessor Primitive::joints(size_t index) const noexcept {
        assert(index < 10);
        char attr[] = "JOINTS_0";
        attr[7] += static_cast<char>(index);
        return attribute(attr);
    }

    Accessor Primitive::weights(size_t index) const noexcept {
        assert(index < 10);
        char attr[] = "WEIGHTS_0";
        attr[8] += static_cast<char>(index);
        return attribute(attr);
    }

    Accessor Primitive::indices() const noexcept {
        size_t index;
        if (findNumber(ptr, "indices", index)) {
            return gltf->accessor(index);
        }
        return Accessor();
    }

    Material Primitive::material() const noexcept {
        size_t num;
        if (findNumber<size_t>(ptr, "material", num)) {
            return gltf->material(num);
        }
        return Material();
    }

    MorphTarget Primitive::target(size_t index) const noexcept {
        return findObject<MorphTarget>(gltf, ptr, "targets", index);
    }

    size_t Primitive::targetCount() const noexcept {
        return count("targets");
    }

    Accessor::Type Accessor::type() const noexcept {
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

    BufferView Accessor::bufferView() const noexcept {
        size_t num;
        if (findNumber<size_t>(ptr, "bufferView", num)) {
            return gltf->bufferView(num);
        }
        return BufferView();
    }

    size_t Accessor::byteOffset() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "byteOffset", 0);
    }

    Accessor::ComponentType Accessor::componentType() const noexcept {
        int num;
        if (findNumber<int>(ptr, "componentType", num)) {
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

    bool Accessor::normalized() const noexcept {
        return findBool("normalized");
    }

    size_t Accessor::count() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "count", 0);
    }

    bool Accessor::max(float* p, size_t count) const noexcept {
        return copyFloats("max", count, p);
    }

    float Accessor::max(size_t index) const noexcept {
        return findNumberOrDefault(ptr, "max", index, 0.0f);
    }

    size_t Accessor::maxCount() const noexcept {
        return Base::count("max");
    }

    bool Accessor::min(float* p, size_t count) const noexcept {
        return copyFloats("max", count, p);
    }

    float Accessor::min(size_t index) const noexcept {
        return findNumberOrDefault(ptr, "min", index, 0.0f);
    }

    size_t Accessor::minCount() const noexcept {
        return Base::count("min");
    }

    Sparse Accessor::sparse() const noexcept {
        return findObject<Sparse>(gltf, ptr, "sparse");
    }

    const char* Asset::copyright() const noexcept {
        return str("copyright");
    }

    const char* Asset::generator() const noexcept {
        return str("generator");
    }

    const char* Asset::version() const noexcept {
        return str("version");
    }

    const char* Asset::minVersion() const noexcept {
        return str("minVersion");
    }

    const char* Buffer::uri() const noexcept {
        return str("uri");
    }

    size_t Buffer::byteLength() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "byteLength", 0);
    }

    Buffer BufferView::buffer() const noexcept {
        size_t num;
        if (findNumber(ptr, "buffer", num)) {
            return gltf->buffer(num);
        }
        return Buffer();
    }

    size_t BufferView::byteOffset() const noexcept {
        return findNumberOrDefault(ptr, "byteOffset", 0);
    }

    size_t BufferView::byteLength() const noexcept {
        return findNumberOrDefault(ptr, "byteLength", 1);
    }

    size_t BufferView::byteStride() const noexcept {
        return findNumberOrDefault(ptr, "byteStride", 4);
    }

    BufferView::Target BufferView::target() const noexcept {
        int num;
        if (findNumber<int>(ptr, "target", num)) {
            switch (num) {
            case GLValue::ELEMENT_ARRAY_BUFFER:
                return Target::ELEMENT_ARRAY_BUFFER;;
            case GLValue::ARRAY_BUFFER:
                return Target::ARRAY_BUFFER;
            }
        }
        return Target::ARRAY_BUFFER;
    }

    bool BufferView::hasTarget() const noexcept {
        int num;
        return (findNumber<int>(ptr, "target", num));
    }

    const char* Image::uri() const noexcept {
        return str("uri");
    }

    const char* Image::mimeType() const noexcept {
        return str("mimeType");
    }

    BufferView Image::bufferView() const noexcept {
        size_t num;
        if (findNumber(ptr, "bufferView", num)) {
            return gltf->bufferView(num);
        }
        return BufferView();
    }

    Image Texture::image() const noexcept {
        size_t num;
        if (findNumber(ptr, "source", num)) {
            return gltf->image(num);
        }
        return Image();
    }

    Image Texture::source() const noexcept {
        return image();
    }

    bool Texture::source(size_t& index) const noexcept {
        return findNumber(ptr, "source", index);
    }

    Sampler Texture::sampler() const noexcept {
        size_t num;
        if (findNumber(ptr, "sampler", num)) {
            return gltf->sampler(num);
        }
        return Sampler();
    }

    Texture TextureInfo::texture() const noexcept {
        if (gltf != nullptr) {
            return gltf->texture(index());
        }
        return Texture();
    }

    size_t TextureInfo::texCoord() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "texCoord", 0);
    }

    size_t TextureInfo::index() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "index", 0);
    }

    Sampler::MagFilter Sampler::magFilter() const noexcept {
        int num;
        if (findNumber(ptr, "magFilter", num)) {
            if (num == GLValue::NEAREST) {
                return MagFilter::NEAREST;
            }
        }
        return MagFilter::LINEAR;
    }

    Sampler::MinFilter Sampler::minFilter() const noexcept {
        int num;
        if (findNumber(ptr, "minFilter", num)) {
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

    Sampler::Wrap Sampler::wrapS() const noexcept {
        int num;
        if (findNumber(ptr, "wrapS", num)) {
            switch (num) {
            case GLValue::CLAMP_TO_EDGE: return Wrap::CLAMP_TO_EDGE;
            case GLValue::MIRRORED_REPEAT: return Wrap::MIRRORED_REPEAT;
            case GLValue::REPEAT: return Wrap::REPEAT;
            }
        }
        return Wrap::REPEAT;
    }

    Sampler::Wrap Sampler::wrapT() const noexcept {
        int num;
        if (findNumber(ptr, "wrapT", num)) {
            switch (num) {
            case GLValue::CLAMP_TO_EDGE: return Wrap::CLAMP_TO_EDGE;
            case GLValue::MIRRORED_REPEAT: return Wrap::MIRRORED_REPEAT;
            case GLValue::REPEAT: return Wrap::REPEAT;
            }
        }
        return Wrap::REPEAT;
    }

    Accessor Skin::inverseBindMatrices() const noexcept {
        size_t num;
        if (findNumber<size_t>(ptr, "inverseBindMatrices", num)) {
            return gltf->accessor(num);
        }
        return Accessor();
    }

    Node Skin::skeleton() const noexcept {
        size_t num;
        if (findNumber<size_t>(ptr, "skeleton", num)) {
            return gltf->node(num);
        }
        // Spec says: "When undefined, joints transforms resolve to scene root."
        // I'll leave it up to the caller to find the root node.
        return Node();
    }

    bool Skin::joint(size_t* p, size_t count) const noexcept {
        return copyNumbers<size_t>(ptr, "joints", count, p);
    }

    size_t Skin::joint(size_t index) const noexcept {
        return findNumberOrDefault<size_t>(ptr, "joints", index, 0);
    }

    size_t Skin::jointCount() const noexcept {
        return Base::count("joints");
    }

    PbrMetallicRoughness Material::pbrMetallicRoughness() const noexcept {
        return findObject<PbrMetallicRoughness>(gltf, ptr, "pbrMetallicRoughness");
    }

    NormalTextureInfo Material::normalTexture() const noexcept {
        return findObject<NormalTextureInfo>(gltf, ptr, "normalTexture");
    }

    OcclusionTextureInfo Material::occlusionTexture() const noexcept {
        return findObject<OcclusionTextureInfo>(gltf, ptr, "occlusionTexture");
    }

    TextureInfo Material::emissiveTexture() const noexcept {
        return findObject<TextureInfo>(gltf, ptr, "emissiveTexture");
    }

    bool Material::emissiveFactor(float* emissive) const noexcept {
        return copyFloats("emissiveFactor", 3, emissive);
    }

    Material::AlphaMode Material::alphaMode() const noexcept {
        const char* s = str("alphaMode");
        if (s != nullptr) {
            switch (*s) {
            case 'M': return AlphaMode::MASK;
            case 'B': return AlphaMode::BLEND;
            }
        }
        return AlphaMode::OPAQUE;
    }

    float Material::alphaCutoff() const noexcept {
        return findFloat("alphaCutoff", 0.5f);
    }

    bool Material::doubleSided() const noexcept {
        return findBool("doubleSided");
    }

    float NormalTextureInfo::scale() const noexcept {
        return findFloat("scale", 1.0f);
    }

    float OcclusionTextureInfo::strength() const noexcept {
        return findFloat("strength", 1.0f);
    }

    bool PbrMetallicRoughness::baseColorFactor(float* color) const noexcept {
        if (color == nullptr) {
            return false;
        }
        if (!copyFloats("baseColorFactor", 4, color)) {
            // failed read baseColorFactor so set to 1,1,1,1
            std::fill(color, color + 4, 1.0f);
        }
        return true;
    }

    TextureInfo PbrMetallicRoughness::baseColorTexture() const noexcept {
        return findObject<TextureInfo>(gltf, ptr, "baseColorTexture");
    }

    float PbrMetallicRoughness::metallicFactor() const noexcept {
        return findFloat("metallicFactor", 1.0f);
    }

    float PbrMetallicRoughness::roughnessFactor() const noexcept {
        return findFloat("roughnessFactor", 1.0f);
    }

    TextureInfo PbrMetallicRoughness::metallicRoughnessTexture() const noexcept {
        return findObject<TextureInfo>(gltf, ptr, "metallicRoughnessTexture");
    }

    Channel Animation::channel(size_t index) const noexcept {
        return findChannel(gltf, ptr, index);
    }

    size_t Animation::channelCount() const noexcept {
        return count("channels");
    }

    AnimationSampler Animation::sampler(size_t index) const noexcept {
        return findObject<AnimationSampler>(gltf, ptr, "samplers", index);
    }

    size_t Animation::samplerCount() const noexcept {
        return count("samplers");
    }

    Accessor AnimationSampler::input() const noexcept {
        size_t index;
        if (input(index)) {
            return gltf->accessor(index);
        }
        return Accessor();
    }

    bool AnimationSampler::input(size_t& index) const noexcept {
        return findNumber(ptr, "input", index);
    }

    Accessor AnimationSampler::output() const noexcept {
        size_t index;
        if (output(index)) {
            return gltf->accessor(index);
        }
        return Accessor();
    }

    bool AnimationSampler::output(size_t & index) const noexcept {
        return findNumber(ptr, "output", index);
    }

    Interpolation AnimationSampler::interpolation() const noexcept {
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

    const char* AnimationSampler::interpolationStr() const noexcept {
        return str("interpolation");
    }

    AnimationSampler Channel::sampler() const noexcept {
        size_t index;
        if (sampler(index)) {
            assert(animation);
            return findObject<AnimationSampler>(gltf, animation, "samplers", index);
        }
        return AnimationSampler();
    }

    bool Channel::sampler(size_t& index) const noexcept {
        return findNumber(ptr, "sampler", index);
    }

    Channel::Target Channel::target() const noexcept {
        return findObject<Channel::Target>(gltf, ptr, "target");
    }

    Node Channel::Target::node() const noexcept {
        size_t index;
        if (node(index)) {
            return gltf->node(index);
        }
        return Node();
    }

    bool Channel::Target::node(size_t& index) const noexcept {
        return findNumber(ptr, "node", index);
    }

    TargetPath Channel::Target::path() const noexcept {
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

    const char* Channel::Target::pathStr() const noexcept {
        return str("path");
    }

    size_t Sparse::count() const noexcept {
        return findNumberOrDefault<size_t>(ptr, "count", 1);
    }

    SparseIndices Sparse::indices() const noexcept {
        return findObject<SparseIndices>(gltf, ptr, "indices");
    }

    SparseValues Sparse::values() const noexcept {
        return findObject<SparseValues>(gltf, ptr, "values");
    }

    BufferView SparseIndices::bufferView() const noexcept {
        size_t index;
        if (findNumber(ptr, "bufferView", index)) {
            return gltf->bufferView(index);
        }
        return BufferView();
    }

    size_t SparseIndices::byteOffset() const noexcept {
        return findNumberOrDefault(ptr, "byteOffset", 0);
    }

    SparseIndices::ComponentType SparseIndices::componentType() const noexcept {
        int num;
        if (findNumber<int>(ptr, "componentType", num)) {
            switch (num) {
            case GLValue::UNSIGNED_BYTE: return ComponentType::UNSIGNED_BYTE;
            case GLValue::UNSIGNED_SHORT: return ComponentType::UNSIGNED_SHORT;
            case GLValue::UNSIGNED_INT: return ComponentType::UNSIGNED_INT;
            }
        }
        return ComponentType::UNSIGNED_INT;
    }

    BufferView SparseValues::bufferView() const noexcept {
        size_t index;
        if (findNumber(ptr, "bufferView", index)) {
            return gltf->bufferView(index);
        }
        return BufferView();
    }

    size_t SparseValues::byteOffset() const noexcept {
        return findNumberOrDefault(ptr, "byteOffset", 0);
    }

    size_t MorphTarget::position() const noexcept {
        return findNumberOrDefault(ptr, "POSITION", 0);
    }

    size_t MorphTarget::normal() const noexcept {
        return findNumberOrDefault(ptr, "NORMAL", 0);
    }

    size_t MorphTarget::tangent() const noexcept {
        return findNumberOrDefault(ptr, "TANGENT", 0);
    }
}
