#pragma once
#ifndef GLTF2_H
#define GLTF2_H

namespace gltf2 {

    class Gltf;
    class Node;
    class Camera;
    class Perspective;
    class Orthographic;
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
    class AnimationSampler;
    class Material;
    class MorphTarget;
    class Image;
    class Skin;
    class NormalTextureInfo;
    class OcclusionTextureInfo;
    class PbrMetallicRoughness;
    class SparseIndices;
    class SparseValues;

    enum class TargetPath {
        TRANSLATION,
        ROTATION,
        SCALE,
        WEIGHTS
    };

    enum class Interpolation {
        LINEAR,
        STEP,
        CATMULLROMSPLINE,
        CUBICSPLINE
    };

    class Base {
    public:
        Base() : gltf(nullptr), ptr(nullptr) {}
        Base(const Gltf* gltf, const void* ptr) : gltf(gltf), ptr(ptr) {}
        virtual ~Base() {}
        bool isNull() const noexcept { return gltf == nullptr; }
        operator bool() const noexcept { return gltf != nullptr; }
    protected:
        const char* str(const char* key) const noexcept;
        /// Returns the size of an array or the number of members in an object.
        size_t count(const char* key) const noexcept;
        bool copyFloats(const char* key, size_t count, float* m) const noexcept;
        float findFloat(const char* key, float defaultValue = 0.0f) const noexcept;
        bool findBool(const char* key, bool defaultValue = false) const noexcept;

        const Gltf* gltf;
        const void* ptr;
    };

    class Named : public Base {
    public:
        Named() {}
        Named(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}
        const char* name() const noexcept;
    };

    // The root glTF object. 
    class Gltf final {
    public:
        Gltf();
        explicit Gltf(const char* path);
        ~Gltf();

        /// Loads a gltf file.
        /// @param[in] path
        /// @return True if the load was successful; false otherwise.
        bool load(const char* path) noexcept;
        operator bool() const noexcept { return _doc != nullptr; }

        bool defaultSceneIndex(size_t& index);

        /// Returns the default scene. This may not be the scene at index zero.
        Scene scene() const noexcept;

        Scene scene(size_t index) const noexcept;
        /// Returns the number of scenes.
        size_t sceneCount() const noexcept;

        Node node(size_t index) const noexcept;
        size_t nodeCount() const noexcept;

        Mesh mesh(size_t index) const noexcept;
        size_t meshCount() const noexcept;

        Camera camera(size_t index) const noexcept;
        size_t cameraCount() const noexcept;

        Accessor accessor(size_t index) const noexcept;
        size_t accessorCount() const noexcept;

        Buffer buffer(size_t index) const noexcept;
        size_t bufferCount() const noexcept;

        BufferView bufferView(size_t index) const noexcept;
        size_t bufferViewCount() const noexcept;

        Animation animation(size_t index) const noexcept;
        size_t animationCount() const noexcept;

        Image image(size_t index) const noexcept;
        size_t imageCount() const noexcept;

        Texture texture(size_t index) const noexcept;
        size_t textureCount() const noexcept;

        Sampler sampler(size_t index) const noexcept;
        size_t samplerCount() const noexcept;

        Material material(size_t index) const noexcept;
        size_t materialCount() const noexcept;

        Skin skin(size_t index) const noexcept;
        size_t skinCount() const noexcept;

        Asset asset() const noexcept;

        /// Returns a pointer to the json document. May be null.
        void* doc() const noexcept;

    private:
        size_t count(const char* key) const noexcept;

        void* _doc;
    };

    class Scene : public Named {
    public:
        Scene() {};
        Scene(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        Node operator[](size_t index) const noexcept;
        Node node(size_t index) const noexcept;
        size_t nodeCount() const noexcept;
        std::vector<size_t> nodes() const noexcept;

    };

    class Node : public Named {
    public:
        Node() {};
        Node(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        Node operator[](size_t index) const noexcept;
        Node child(size_t index) const noexcept;
        size_t childCount() const noexcept;
        std::vector<size_t> children() const noexcept;
        std::vector<size_t> nodes() const noexcept;

        bool matrix(float* m) const noexcept;
        bool translation(float* m) const noexcept;
        bool rotation(float* m) const noexcept;
        bool scale(float* m) const noexcept;

        bool weight(float* p, size_t count) const noexcept;
        float weight(size_t index) const noexcept;
        size_t weightCount() const noexcept;

        Mesh mesh() const noexcept;
        bool mesh(size_t& index) const noexcept;

        Camera camera() const noexcept;
        bool camera(size_t& index) const noexcept;

        Skin skin() const noexcept;
        bool skin(size_t& index) const noexcept;
    };

    class Camera : public Named {
    public:
        Camera() {}
        Camera(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        enum class Type {
            ORTHOGRAPHIC,
            PERSPECTIVE
        };

        Type type() const noexcept;
        Orthographic orthographic() const noexcept;
        Perspective perspective() const noexcept;
    };

    class Orthographic : public Base {
    public:
        Orthographic() {}
        Orthographic(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        float xmag() const noexcept;
        float ymag() const noexcept;
        float zfar() const noexcept;
        float znear() const noexcept;
    };

    class Perspective : public Base {
    public:
        Perspective() {}
        Perspective(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        float aspectRatio() const noexcept;
        float yfov() const noexcept;
        float zfar() const noexcept;
        float znear() const noexcept;
    };

    class Mesh : public Named {
    public:
        Mesh() {}
        Mesh(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        Primitive primitive(size_t index) const noexcept;
        size_t primitiveCount() const noexcept;

        bool weight(float* p, size_t count) const noexcept; // TODO rename to weights
        float weight(size_t index) const noexcept;
        size_t weightCount() const noexcept;
    };

    class Primitive : public Base {
    public:

        enum class Mode {
            POINTS = 0,
            LINES = 1,
            LINE_LOOP = 2,
            LINE_STRIP = 3,
            TRIANGLES = 4,
            TRIANGLE_STRIP = 5,
            TRIANGLE_FAN = 6
        };

        Primitive() {}
        Primitive(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        Mode mode() const noexcept;

        Accessor attribute(const char* attribute) const noexcept;
        /// Returns the list of attributes as a vector of pairs.
        /// Pair.first is the attribute name and pair.second is the Accessor index.
        std::vector<std::pair<const char*, size_t>> attributes() const noexcept;

        /// Returns the number of attributes in this primitive.
        size_t attributeCount() const noexcept;
        /// Returns a list of attribute names from this primitive.
        /// The char pointers in this list will be invalid when the root gltf object is destroyed 
        /// or loads a new file. 
        std::vector<const char*> attributeStrings() const noexcept;
        Accessor position() const noexcept;
        Accessor normal() const noexcept;
        Accessor tangent() const noexcept;
        Accessor texcoord(size_t index) const noexcept;
        Accessor color(size_t index = 0) const noexcept;
        Accessor joints(size_t index = 0) const noexcept;
        Accessor weights(size_t index = 0) const noexcept;
        Accessor indices() const noexcept;
        bool indices(size_t& index) const noexcept;
        Material material() const noexcept;
        bool material(size_t& index) const noexcept;
        MorphTarget target(size_t index) const noexcept;
        size_t targetCount() const noexcept;

    };

    class Accessor : public Named {
    public:
        Accessor() {}
        Accessor(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

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

        Type type() const noexcept;
        BufferView bufferView() const noexcept;
        bool bufferView(size_t& index) const noexcept;
        size_t byteOffset() const noexcept;
        ComponentType componentType() const noexcept;
        bool normalized() const noexcept;
        size_t count() const noexcept;
        bool max(float* p, size_t count) const noexcept;
        float max(size_t index) const noexcept;
        size_t maxCount() const noexcept;
        bool min(float* p, size_t count) const noexcept;
        float min(size_t index) const noexcept;
        size_t minCount() const noexcept;
        Sparse sparse() const noexcept;
    };

    class Asset : public Base {
        friend Gltf;
    public:
        Asset() {};
        const char* copyright() const noexcept;
        const char* generator() const noexcept;
        const char* version() const noexcept;
        const char* minVersion() const noexcept;
    private:
        Asset(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}
    };

    class Buffer : public Named {
    public:
        Buffer() {}
        Buffer(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        const char* uri() const noexcept;
        size_t byteLength() const noexcept;
        // TODO method to get values regardless of location
    };

    class BufferView : public Named {
    public:
        BufferView() {}
        BufferView(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        enum Target {
            ARRAY_BUFFER = 34962,
            ELEMENT_ARRAY_BUFFER = 34963
        };

        Buffer buffer() const noexcept;
        bool buffer(size_t& index) const noexcept;
        size_t byteOffset() const noexcept;
        size_t byteLength() const noexcept;
        size_t byteStride() const noexcept;
        Target target() const noexcept;
        bool hasTarget() const noexcept;
    };

    class Image : public Named {
    public:
        Image() {}
        Image(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        const char* uri() const noexcept;
        const char* mimeType() const noexcept;
        BufferView bufferView() const noexcept;
    };

    class Texture : public Named {
    public:
        Texture() {}
        Texture(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        /// Returns the Image used by this texture. This method name is more obvious.
        Image image() const noexcept;
        /// Returns the Image used by this texture. Same as image().
        Image source() const noexcept;

        /// Gets the index of the image used by this texture.
        /// @param[out] index The variable to copy the index to.
        /// @return True if the source index of found.
        bool source(size_t& index) const noexcept;
        Sampler sampler() const noexcept;
    };

    class TextureInfo : public Base {
    public:
        TextureInfo() {}
        TextureInfo(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        /// Returns the texture instead of having to use index().
        Texture texture() const noexcept;
        size_t texCoord() const noexcept;
        /// The index of the texture (when using Gltf::texture(size_t))
        size_t index() const noexcept;
        //bool index(size_t& index) const noexcept; // TODO
    };

    class Sampler : public Named {
    public:
        Sampler() {}
        Sampler(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

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

        MagFilter magFilter() const noexcept;
        MinFilter minFilter() const noexcept;
        Wrap wrapS() const noexcept;
        Wrap wrapT() const noexcept;
    };

    class Skin : public Named {
    public:
        Skin() {}
        Skin(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        Accessor inverseBindMatrices() const noexcept;
        Node skeleton() const noexcept;
        bool joints(size_t* p, size_t count) const noexcept;
        size_t joint(size_t index) const noexcept;
        size_t jointCount() const noexcept;
        std::vector<size_t> joints() const noexcept;
    };

    class Material : public Named {
    public:
        Material() {}
        Material(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}

        enum class AlphaMode {
            OPAQUE,
            MASK,
            BLEND
        };

        PbrMetallicRoughness pbrMetallicRoughness() const noexcept;
        NormalTextureInfo normalTexture() const noexcept;
        OcclusionTextureInfo occlusionTexture() const noexcept;
        TextureInfo emissiveTexture() const noexcept;
        bool emissiveFactor(float* emissive) const noexcept;
        AlphaMode alphaMode() const noexcept;
        float alphaCutoff() const noexcept;
        bool doubleSided() const noexcept;
    };

    class NormalTextureInfo : public TextureInfo {
    public:
        NormalTextureInfo() {}
        NormalTextureInfo(const Gltf* gltf, const void* ptr) : TextureInfo(gltf, ptr) {}

        float scale() const noexcept;
    };

    class OcclusionTextureInfo : public TextureInfo {
    public:
        OcclusionTextureInfo() {}
        OcclusionTextureInfo(const Gltf* gltf, const void* ptr) : TextureInfo(gltf, ptr) {}

        float strength() const noexcept;
    };

    class PbrMetallicRoughness : public Base {
    public:
        PbrMetallicRoughness() {}
        PbrMetallicRoughness(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}
        // float[4] baseColorFactor;
        // textureInfo baseColorTexture;
        // float metallicFactor;
        // float roughnessFactor;
        // textureInfo metallicRoughnessTexture;

        /// Copies 4 floats to the given pointer.
        bool baseColorFactor(float* color) const noexcept;
        TextureInfo baseColorTexture() const noexcept;
        float metallicFactor() const noexcept;
        float roughnessFactor() const noexcept;
        TextureInfo metallicRoughnessTexture() const noexcept;
    };

    class Animation : public Named {
    public:
        Animation() {}
        Animation(const Gltf* gltf, const void* ptr) : Named(gltf, ptr) {}
        // vector<channel> channels;
        // vector<animation_sampler> samplers;
        // string name;

        Channel channel(size_t index) const noexcept;
        size_t channelCount() const noexcept;
        AnimationSampler sampler(size_t index) const noexcept;
        size_t samplerCount() const noexcept;
    };

    class AnimationSampler : public Base {
    public:
        AnimationSampler() {}
        AnimationSampler(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}
        // int input;
        // string interpolation;
        // int output;

        Accessor input() const noexcept;
        bool input(size_t& index) const noexcept;
        Accessor output() const noexcept;
        bool output(size_t& index) const noexcept;
        Interpolation interpolation() const noexcept;
        const char* interpolationStr() const noexcept;
    };

    class Channel : public Base {
    public:
        Channel() {}
        Channel(const Gltf* gltf, const void* ptr, const void* animation) : Base(gltf, ptr), animation(animation) {}

        class Target : public Base {
        public:
            Target() {}
            Target(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

            Node node() const noexcept;
            bool node(size_t& index) const noexcept;
            TargetPath path() const noexcept;
            const char* pathStr() const noexcept;
        };

        AnimationSampler sampler() const noexcept;
        bool sampler(size_t& index) const noexcept;
        Channel::Target target() const noexcept;
    private:
        /// Pointer to the parent animation json object
        const void* animation;
    };

    class Sparse : public Base {
    public:
        Sparse() {}
        Sparse(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        size_t count() const noexcept;
        SparseIndices indices() const noexcept;
        SparseValues values() const noexcept;
    };

    class SparseIndices : public Base {
    public:
        SparseIndices() {}
        SparseIndices(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        enum class ComponentType {
            UNSIGNED_BYTE = 5121,
            UNSIGNED_SHORT = 5123,
            UNSIGNED_INT = 5125
        };

        BufferView bufferView() const noexcept;
        size_t byteOffset() const noexcept;
        ComponentType componentType() const noexcept;
    };

    class SparseValues : public Base {
    public:
        SparseValues() {}
        SparseValues(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        BufferView bufferView() const noexcept;
        size_t byteOffset() const noexcept;
    };

    class MorphTarget : public Base {
    public:
        MorphTarget() {}
        MorphTarget(const Gltf* gltf, const void* ptr) : Base(gltf, ptr) {}

        size_t position() const noexcept;
        size_t normal() const noexcept;
        size_t tangent() const noexcept;
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
}

#endif
