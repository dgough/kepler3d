#include "stdafx.h"
#include "GLTF2Loader.hpp"

#include "gltf2.hpp"

#include "Scene.hpp"
#include "Camera.hpp"
#include "Node.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "MeshRenderer.hpp"
#include "VertexAttributeAccessor.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "IndexAccessor.hpp"
#include "Material.hpp"
#include "MaterialParameter.hpp"
#include "Effect.hpp"
#include "Technique.hpp"
#include "Sampler.hpp"
#include "Image.hpp"
#include "FileSystem.hpp"
#include "StringUtils.hpp"
#include "Logging.hpp"

#include <json.hpp>
#include <chrono>
#include <array>

#define BUFFERSIZE 1024
#include <b64/decode.h>

using std::string;
using std::shared_ptr;
using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;

#define RETURN_IF_FOUND(map, key) \
    { \
        auto i = (map).find(key); \
        if (i != (map).end()) { \
            return i->second; \
        } \
    }

static constexpr GLchar* DEFAULT_VERT_SHADER = "precision highp float;\n"
"\n"
"uniform mat4 u_modelViewMatrix;\n"
"uniform mat3 u_normalMatrix;\n"
"uniform mat4 u_projectionMatrix;\n"
"\n"
"attribute vec3 a_normal;\n"
"attribute vec3 a_position;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_Position = u_projectionMatrix * u_modelViewMatrix * vec4(a_position,1.0);\n"
"}\n";

static constexpr GLchar* DEFAULT_FRAG_SHADER = "precision highp float;\n"
"\n"
"uniform vec4 u_emission;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = u_emission;\n"
"}\n";

namespace kepler {

    using ubyte = unsigned char;

    static const string BLANK_STR = "";
    static const string NODES = "nodes";
    static const string SCENE = "scene";
    static const string SCENES = "scenes";
    static const string NAME = "name";
    static const string CHILDREN = "children";
    static const string TRANSLATION = "translation";
    static const string ROTATION = "rotation";
    static const string SCALE = "scale";
    static const string MATRIX = "matrix";
    static const string MESHES = "meshes";
    static const string PRIMITIVES = "primitives";
    static const string MODE = "mode";
    static const string INDICES = "indices";
    static const string ATTRIBUTES = "attributes";
    static const string CAMERA = "camera";
    static const string CAMERAS = "cameras";
    static const string TYPE = "type";
    static const string PERSPECTIVE = "perspective";
    static const string ORTHOGRAPHIC = "orthographic";
    static const string MATERIAL = "material";
    static const string MATERIALS = "materials";
    static const string TECHNIQUE = "technique";
    static const string TECHNIQUES = "techniques";

    static const string ACCESSORS = "accessors";
    static const string BUFFER = "buffer";
    static const string BUFFERS = "buffers";
    static const string BUFFER_VIEW = "bufferView";
    static const string BUFFER_VIEWS = "bufferViews";
    static const string ARRAY_BUFFER = "arraybuffer";
    static const string TEXT = "text";
    static const string URI = "uri";

    static const string SEMANTIC = "semantic";
    static const string DATA_APP_BASE64 = "data:application/octet-stream;base64,";
    static const string DATA_IMAGE_BASE64 = "data:image/";
    static const string DATA_TEXT_BASE64 = "data:text/plain;base64,";

    static constexpr float DEFAULT_ASPECT_RATIO = 1.5f;
    static constexpr float DEFAULT_NEAR = 0.1f;
    static constexpr float DEFAULT_FAR = 100.0f;
    static constexpr float DEFAULT_FOVY = PI * 0.25f;

    static constexpr int DEFAULT_FORMAT = GL_RGBA;
    static constexpr int DEFAULT_TARGET = GL_TEXTURE_2D;
    static constexpr int DEFAULT_TEXEL_TYPE = GL_UNSIGNED_BYTE;

    static constexpr int DEFAULT_MAG_FILTER = GL_LINEAR;
    static constexpr int DEFAULT_MIN_FILTER = GL_NEAREST_MIPMAP_LINEAR;
    static constexpr int DEFAULT_WRAP_S = GL_REPEAT;
    static constexpr int DEFAULT_WRAP_T = GL_REPEAT;

    // functions
    static MeshPrimitive::Mode toMode(int mode);
    static MaterialParameter::Semantic toSemantic(const string& semantic);
    static AttributeSemantic toAttributeSemantic(const string& semantic);
    static Sampler::Wrap toWrapMode(int wrap);
    static Sampler::MinFilter toMinFilterMode(int filter);
    static Sampler::MagFilter toMagFilterMode(int filter);
    static GLint numComponentsOfType(const string& type);
    static void setState(int state, RenderState& block);
    static void setFunction(RenderState& renderState, const string& key, const json& value);
    static void base64Decode(const string& text, size_t offset, string& destination);
    static void base64Decode(std::istream& istream, std::vector<ubyte>& out);
    static shared_ptr<std::vector<ubyte>> createBufferFromBase64(const string& text, size_t offset);

    using time_type = std::chrono::nanoseconds;
    static time_type __totalTime;

    // Private implementation
    /// @Internal
    class GLTF2Loader::Impl final {
        friend class GLTF2Loader;
    public:
        Impl();
        ~Impl() noexcept;

        bool loadJson(const char* path);

        ref<Scene> loadSceneFromFile(const char* path);

        ref<Scene> loadDefaultScene();

        ref<Scene> loadScene(size_t index);
        ref<Scene> loadScene(const string& id);

        ref<Node> loadNode(size_t index);
        ref<Node> loadNode(const string& id);
        ref<Node> loadNode(const gltf2::Node& gNode);
        ref<Node> loadNode(const json& jNode);

        ref<Mesh> loadMesh(const string& id);
        ref<Camera> loadCamera(const string& id);

        shared_ptr<std::vector<ubyte>> loadBuffer(const string& id);

        ref<VertexBuffer> loadVertexBuffer(const string& id);
        ref<IndexBuffer> loadIndexBuffer(const string& id);
        ref<IndexAccessor> loadIndexAccessor(const string& id);
        ref<VertexAttributeAccessor> loadVertexAttributeAccessor(const string& id);

        ref<Material> loadMaterial(const string& id);
        ref<Technique> loadTechnique(const string& id);
        void loadTechniqueAttribute(const string& glslName, const string& paramName, const json& parameters, ref<Technique> tech);
        void loadTechniqueUniform(const string& glslName, const string& paramName, const json& parameters, ref<Technique> tech);
        ref<Effect> loadProgram(const string& id);

        ref<Texture> loadTexture(const string& id);
        ref<Sampler> loadSampler(const string& id);
        ref<Image> loadImage(const string& id);

        ref<Material> loadDefaultMaterial();
        ref<Technique> loadDefaultTechnique();

        void loadShaderSource(const string& id, string& destination);

        // load by name
        ref<Material> loadMaterialByName(const string& name);

        string uriToPath(const string& uri) const;

        void useDefaultMaterial(bool value);
        void setAutoLoadMaterials(bool value);

    private:
        void loadTransform(const gltf2::Node& gNode, const ref<Node>& node);
        void loadTransform(const json& jNode, ref<Node> node);


    private:
        gltf2::Gltf _gltf;
        json _json; // TODO remove

        std::map<size_t, ref<Node>> _nodes;


        std::map <string, std::shared_ptr<std::vector<ubyte>>> _buffersOld;

        std::map<string, ref<Node>> _nodesOld;
        std::map<string, ref<VertexBuffer>> _vbosOld;
        std::map<string, ref<IndexBuffer>> _indexBuffersOld;
        std::map<string, ref<IndexAccessor>> _indexAccessorsOld;
        std::map<string, ref<VertexAttributeAccessor>> _vertexAttributeAccessorsOld;

        std::map<string, ref<Material>> _materialsOld;
        std::map<string, ref<Technique>> _techniquesOld;
        std::map<string, ref<Effect>> _effectsOld;
        std::map<string, ref<Texture>> _texturesOld;
        std::map<string, ref<Sampler>> _samplersOld;
        std::map<string, ref<Image>> _imagesOld;

        ref<Material> _defaultMaterial;
        ref<Technique> _defaultTechnique;

        string _baseDir;

        bool _loaded;
        bool _useDefaultMaterial;
        bool _autoLoadMaterials;
        float _aspectRatio;

        time_type _jsonLoadTime;
    };

    ////////////////////////////////////////////////////////////////

    GLTF2Loader::GLTF2Loader() : _impl(std::make_unique<Impl>()) {
    }

    GLTF2Loader::GLTF2Loader(const char * path) : _impl(std::make_unique<Impl>()) {
        _impl->loadJson(path);
    }

    GLTF2Loader::~GLTF2Loader() noexcept {
    }

    GLTF2Loader::operator bool() const {
        return _impl->_loaded;
    }

    bool GLTF2Loader::load(const char* path) {
        return _impl->loadJson(path);
    }

    ref<Scene> GLTF2Loader::loadSceneFromFile(const char* path) {
        return _impl->loadSceneFromFile(path);
    }

    ref<Material> GLTF2Loader::findMaterialById(const std::string& id) {
        return _impl->loadMaterial(id);
    }

    ref<Material> GLTF2Loader::findMaterialByName(const std::string& name) {
        return _impl->loadMaterialByName(name);
    }

    ref<Mesh> GLTF2Loader::findMeshById(const std::string& id) {
        return _impl->loadMesh(id);
    }

    void GLTF2Loader::clear() {
        _impl.reset();
        _impl = std::make_unique<Impl>();
    }

    void GLTF2Loader::useDefaultMaterial(bool value) {
        _impl->useDefaultMaterial(value);
    }

    void GLTF2Loader::setAutoLoadMaterials(bool value) {
        _impl->setAutoLoadMaterials(value);
    }

    void GLTF2Loader::setCameraAspectRatio(float aspectRatio) {
        _impl->_aspectRatio = aspectRatio;
    }

    void GLTF2Loader::printTotalTime() {
        std::clog << "Total: " << std::chrono::duration_cast<std::chrono::milliseconds>(__totalTime).count() << " ms" << std::endl;
    }

    ////////////////////////////////////////////////////////////////

    GLTF2Loader::Impl::Impl()
        : _loaded(false), _useDefaultMaterial(false), _autoLoadMaterials(true), _aspectRatio(0.0f) {
    }

    GLTF2Loader::Impl::~Impl() noexcept {
    }

    bool GLTF2Loader::Impl::loadJson(const char* path) {
        _baseDir = directoryName(path);

        auto start = std::chrono::system_clock::now();

        _loaded = _gltf.load(path);

        auto end = std::chrono::system_clock::now();
        _jsonLoadTime = std::chrono::duration_cast<time_type>(end - start);

        return _loaded;
    }

    ref<Scene> GLTF2Loader::Impl::loadSceneFromFile(const char* path) {
        // TODO call clear() first?
        auto start = std::chrono::system_clock::now();

        if (!loadJson(path)) {
            loge("LOAD_SCENE_FROM_FILE ", path);
            return nullptr;
        }

        auto scene = loadDefaultScene();

        auto end = std::chrono::system_clock::now();
        auto t = std::chrono::duration_cast<time_type>(end - start);
        __totalTime += t;
        auto clogWidth = std::clog.width();
        std::clog.width(8);
        std::clog << std::chrono::duration_cast<std::chrono::milliseconds>(t).count() << " ms to load ";

        std::clog << "(";
        double percent = (double)_jsonLoadTime.count() / (double)t.count() * 100.0;
        std::clog.width(2);
        std::clog << std::fixed;
        std::clog << std::setprecision(0);
        std::clog << percent << "% json) ";
        std::clog << path << std::endl;
        std::clog.unsetf(std::ios_base::floatfield);
        std::clog.width(clogWidth);
        return scene;
    }

    ref<Scene> GLTF2Loader::Impl::loadDefaultScene() {
        size_t index;
        if (_gltf.defaultSceneIndex(index)) {
            return loadScene(index);
        }
        // TODO load the first scene in the "scenes" list?
        return nullptr;
    }

    ref<Scene> GLTF2Loader::Impl::loadScene(size_t index) {
        if (auto gScene = _gltf.scene(index)) {
            auto scene = Scene::create();
            for (const auto& index : gScene.nodes()) {
                scene->addNode(loadNode(index));
            }
            return scene;
        }
        return nullptr;
    }

    ref<Scene> GLTF2Loader::Impl::loadScene(const string& id) {
        auto jScenes = _json.find(SCENES);
        if (jScenes != _json.end()) {
            auto jScene = jScenes->find(id);
            if (jScene != jScenes->end()) {
                auto jNodes = jScene->find(NODES);
                if (jNodes->is_array()) {
                    auto scene = Scene::create();
                    for (auto& jNode : *jNodes) {
                        scene->addNode(loadNode(jNode.get_ref<const string&>()));
                    }
                    // TODO active camera?
                    return scene;
                }
            }
        }
        return nullptr;
    }

    ref<Node> GLTF2Loader::Impl::loadNode(size_t index) {
        const auto it = _nodes.find(index); // TODO make macro for this
        if (it != _nodes.end()) {
            return it->second;
        }
        if (auto gNode = _gltf.node(index)) {
            auto node = loadNode(gNode);
            _nodes[index] = node;
            return node;
        }
        return nullptr;
    }

    ref<Node> GLTF2Loader::Impl::loadNode(const string& id) {
        RETURN_IF_FOUND(_nodesOld, id);
        auto jNodes = _json.find(NODES);
        if (jNodes != _json.end()) {
            auto jNode = jNodes->find(id);
            if (jNode != jNodes->end()) {
                auto node = loadNode(*jNode);
                _nodesOld[id] = node;
                return node;
            }
        }
        return nullptr;
    }

    ref<Node> GLTF2Loader::Impl::loadNode(const gltf2::Node& gNode) {
        auto node = Node::create(gNode.name());

        loadTransform(gNode, node);

        // load mesh
        // load camera
        // load children
        for (const auto& index : gNode.children()) {
            node->addNode(loadNode(index));
        }
        return node;
    }

    ref<Node> GLTF2Loader::Impl::loadNode(const json& jNode) {
        auto name = jNode.value(NAME, "");
        auto node = Node::create(name);

        loadTransform(jNode, node);

        // load mesh
        auto meshes = jNode.find(MESHES);
        if (meshes != jNode.end() && meshes->is_array() && meshes->size() > 0) {
            // TODO for now, only load one mesh
            const auto& meshId = meshes->at(0);
            auto mesh = loadMesh(meshId.get_ref<const string&>());
            if (mesh) {
                node->addComponent(MeshRenderer::create(mesh));
            }
        }

        // load camera
        auto cameraId = jNode.find(CAMERA);
        if (cameraId != jNode.end()) {
            node->addComponent(loadCamera(cameraId->get_ref<const string&>()));
        }

        // load children
        auto children = jNode.find(CHILDREN);
        if (children->is_array() && children->size() > 0) {
            for (const auto& child : *children) {
                if (child.is_string()) {
                    node->addNode(loadNode(child.get_ref<const string&>()));
                }
            }
        }
        return node;
    }

    ref<Mesh> GLTF2Loader::Impl::loadMesh(const string& id) {
        try {
            auto jMeshes = _json.find(MESHES);
            if (jMeshes != _json.end()) {
                auto jMesh = jMeshes->find(id);
                if (jMesh != jMeshes->end()) {
                    auto jPrimitives = jMesh->find(PRIMITIVES);
                    if (jPrimitives != jMesh->end()) {
                        auto mesh = Mesh::create();
                        mesh->setName(jMesh->value(NAME, ""));
                        // load primitives
                        for (const auto& jPrimitive : *jPrimitives) {
                            int mode = jPrimitive[MODE].get<int>();
                            auto meshPrimitive = MeshPrimitive::create(toMode(mode));

                            auto jAttribs = jPrimitive.find(ATTRIBUTES);
                            if (jAttribs != jPrimitive.end() && jAttribs->is_object()) {
                                for (auto jAttrib = jAttribs->begin(); jAttrib != jAttribs->end(); ++jAttrib) {
                                    auto vertexAttributeAccessor = loadVertexAttributeAccessor(jAttrib.value());
                                    if (vertexAttributeAccessor) {
                                        meshPrimitive->setAttribute(toAttributeSemantic(jAttrib.key()), vertexAttributeAccessor);
                                    }
                                }
                            }
                            // load indices
                            auto jIndices = jPrimitive.find(INDICES);
                            if (jIndices != jPrimitive.end()) {
                                auto indexAccessor = loadIndexAccessor(jIndices->get_ref<const string&>());
                                if (indexAccessor) {
                                    meshPrimitive->setIndices(indexAccessor);
                                }
                            }
                            if (_autoLoadMaterials) {
                                // load material
                                auto jMaterialId = jPrimitive.find(MATERIAL);
                                if (jMaterialId != jPrimitive.end()) {
                                    auto material = loadMaterial(jMaterialId->get_ref<const string&>());
                                    if (material) {
                                        meshPrimitive->setMaterial(material);
                                    }
                                }
                                else {
                                    meshPrimitive->setMaterial(loadDefaultMaterial());
                                }
                            }
                            mesh->addMeshPrimitive(meshPrimitive);
                        }
                        if (mesh->primitiveCount() > 0) {
                            return mesh;
                        }
                        else {
                            loge("MESH::ZERO_PRIMITIVES_LOADED");
                        }
                    }
                }
            }
        }
        catch (const std::domain_error&) {
        }
        return nullptr;
    }

    ref<Camera> GLTF2Loader::Impl::loadCamera(const string& id) {
        auto cameras = _json.find(CAMERAS);
        if (cameras != _json.end()) {
            auto camera = cameras->find(id);
            if (camera != cameras->end()) {
                auto type = camera->value(TYPE, "");
                if (type == PERSPECTIVE) {
                    auto p = camera->find(PERSPECTIVE);
                    if (p != camera->end()) {
                        // _aspectRatio can replace the aspect ratio found in the file.
                        float aspectRatio = _aspectRatio != 0.0f ? _aspectRatio : p->value("aspectRatio", DEFAULT_ASPECT_RATIO);
                        float yfov = p->value("yfov", DEFAULT_FOVY);
                        float near = p->value("znear", DEFAULT_NEAR);
                        float far = p->value("zfar", DEFAULT_FAR);
                        return Camera::createPerspective(glm::degrees(yfov), aspectRatio, near, far);
                    }
                }
                else if (type == ORTHOGRAPHIC) {
                    auto p = camera->find(ORTHOGRAPHIC);
                    if (p != camera->end()) {
                        float xmag = p->value("xmag", 0.0f);
                        float ymag = p->value("ymag", 1.0f);
                        float znear = p->value("znear", DEFAULT_NEAR);
                        float zfar = p->value("zfar", DEFAULT_FAR);
                        return Camera::createOrthographic(xmag, ymag, 1.0f, znear, zfar);
                    }
                }
            }
        }
        return nullptr;
    }

    shared_ptr<std::vector<ubyte>> GLTF2Loader::Impl::loadBuffer(const string& id) {
        RETURN_IF_FOUND(_buffersOld, id);

        auto buffers = _json.find(BUFFERS);
        if (buffers != _json.end()) {
            auto jBuffer = buffers->find(id);
            if (jBuffer != buffers->end()) {
                auto type = jBuffer->value(TYPE, ARRAY_BUFFER);
                if (type == ARRAY_BUFFER) {
                    auto jUri = jBuffer->find(URI);
                    if (jUri != jBuffer->end() && jUri->is_string()) {
                        const auto& uri = jUri->get_ref<const string&>();
                        if (startsWith(uri, DATA_APP_BASE64)) {
                            auto buffer = createBufferFromBase64(uri, DATA_APP_BASE64.length());
                            _buffersOld[id] = buffer;
                            return buffer;
                        }
                        else {
                            string path = uriToPath(uri);
                            auto buffer = std::make_shared<std::vector<ubyte>>();
                            if (readBinaryFile(path.c_str(), *buffer.get())) {
                                _buffersOld[id] = buffer;
                                return buffer;
                            }
                        }
                    }
                }
                else if (type == TEXT) {
                    // TODO type="text"?
                }
            }
        }
        // TODO handle error
        return nullptr;
    }

    ref<VertexBuffer> GLTF2Loader::Impl::loadVertexBuffer(const string& id) {
        RETURN_IF_FOUND(_vbosOld, id);
        if (id.empty()) {
            return nullptr;
        }
        auto jViews = _json.find(BUFFER_VIEWS);
        if (jViews != _json.end()) {
            auto jBufferView = jViews->find(id);
            if (jBufferView != jViews->end()) {
                auto jBuffer = jBufferView->find(BUFFER);
                if (jBuffer != jBufferView->end() && jBuffer->is_string()) {
                    const auto& bufferId = jBuffer->get_ref<const string&>();
                    if (bufferId.length() > 0) {
                        auto buffer = loadBuffer(bufferId);
                        if (buffer) {
                            GLsizeiptr byteLength = jBufferView->value<GLsizeiptr>("byteLength", 0);
                            size_t byteOffset = jBufferView->value<size_t>("byteOffset", 0);
                            auto vbo = VertexBuffer::create(byteLength, &(*buffer)[0] + byteOffset, GL_STATIC_DRAW);
                            _vbosOld[id] = vbo;
                            return vbo;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    ref<IndexBuffer> GLTF2Loader::Impl::loadIndexBuffer(const string& id) {
        // TODO is it possible to have more than 1 of the same index buffer?
        RETURN_IF_FOUND(_indexBuffersOld, id);

        // TODO this is very similar to loadVertexBuffer. Move to common function.

        auto jViews = _json.find(BUFFER_VIEWS);
        if (jViews != _json.end()) {
            auto jBufferView = jViews->find(id);
            if (jBufferView != jViews->end()) {
                auto jBuffer = jBufferView->find(BUFFER);
                if (jBuffer != jBufferView->end() && jBuffer->is_string()) {
                    const auto& bufferId = jBuffer->get_ref<const string&>();
                    if (bufferId.length() > 0) {
                        auto buffer = loadBuffer(bufferId);
                        if (buffer) {
                            GLsizeiptr byteLength = jBufferView->value<GLsizeiptr>("byteLength", 0);
                            size_t byteOffset = jBufferView->value<size_t>("byteOffset", 0);
                            auto indexBuffer = IndexBuffer::create(byteLength, &(*buffer)[0] + byteOffset, GL_STATIC_DRAW);
                            _indexBuffersOld[id] = indexBuffer;
                            return indexBuffer;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    ref<IndexAccessor> GLTF2Loader::Impl::loadIndexAccessor(const string& id) {
        RETURN_IF_FOUND(_indexAccessorsOld, id);

        auto jAccessor = _json[ACCESSORS][id];
        if (jAccessor.is_object()) {

            auto indexBuffer = loadIndexBuffer(jAccessor.value(BUFFER_VIEW, ""));
            if (indexBuffer == nullptr) {
                return nullptr;
            }

            //GLint componentSize = numComponentsOfType(jAccessor.value(TYPE, ""));
            GLenum componentType = jAccessor.value<int>("componentType", 0);
            GLintptr byteOffset = jAccessor.value<int>("byteOffset", 0);
            GLsizei count = jAccessor.value<int>("count", 0);

            auto indexAccessor = IndexAccessor::create(indexBuffer, count, componentType, byteOffset);
            //indexAccessor->dump();
            _indexAccessorsOld[id] = indexAccessor;
            return indexAccessor;
        }
        return nullptr;
    }

    ref<VertexAttributeAccessor> GLTF2Loader::Impl::loadVertexAttributeAccessor(const string& id) {
        RETURN_IF_FOUND(_vertexAttributeAccessorsOld, id);

        auto jAccessors = _json.find(ACCESSORS);
        if (jAccessors != _json.end()) {
            auto jAccessor = jAccessors->find(id);
            if (jAccessor != jAccessors->end() && jAccessor->is_object()) {
                auto vbo = loadVertexBuffer(jAccessor->value(BUFFER_VIEW, ""));
                if (vbo == nullptr) {
                    return nullptr;
                }

                GLint componentSize = numComponentsOfType(jAccessor->value(TYPE, ""));
                GLenum componentType = jAccessor->value<int>("componentType", 0);
                GLsizei byteStride = jAccessor->value<int>("byteStride", 0);
                GLintptr byteOffset = jAccessor->value<int>("byteOffset", 0);
                GLsizei count = jAccessor->value<int>("count", 0);

                auto vertexAttributeAccessor = VertexAttributeAccessor::create(vbo, componentSize, componentType, false, byteStride, byteOffset, count);
                _vertexAttributeAccessorsOld[id] = vertexAttributeAccessor;
                return vertexAttributeAccessor;
            }
        }
        return nullptr;
    }

    ref<Material> GLTF2Loader::Impl::loadMaterial(const string& id) {
        RETURN_IF_FOUND(_materialsOld, id);

        if (_useDefaultMaterial) {
            return loadDefaultMaterial();
        }
        auto jMaterials = _json.find(MATERIALS);
        if (jMaterials != _json.end()) {
            auto jMaterial = jMaterials->find(id);
            if (jMaterial != jMaterials->end()) {
                auto material = Material::create();
                material->setName(jMaterial->value(NAME, ""));

                auto jValues = jMaterial->find("values");
                if (jValues != jMaterial->end()) {
                    for (auto v = jValues->begin(); v != jValues->end(); ++v) {

                        auto param = MaterialParameter::create(v.key());

                        auto& value = v.value();
                        if (value.is_number()) {
                            param->setValue(value.get<float>());
                            material->addParam(param);
                        }
                        else if (value.is_array()) {
                            auto size = value.size();
                            if (size == 2) {
                                logw("value size 2 not implemented");
                            }
                            else if (size == 3) {
                                logw("value size 3 not implemented");
                            }
                            else if (size == 4) {
                                glm::vec4 vec(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>());
                                param->setValue(vec);
                                material->addParam(param);
                            }
                        }
                        else if (value.is_string()) {
                            // texture?
                            auto texture = loadTexture(value.get_ref<const string&>());
                            if (texture) {
                                param->setValue(texture);
                                material->addParam(param);
                            }
                        }

                        // TODO need 2, 3 and maybe other sizes
                    }
                }
                auto techniqueId = jMaterial->value(TECHNIQUE, "");
                if (!techniqueId.empty()) {
                    material->setTechnique(loadTechnique(techniqueId));
                    _materialsOld[id] = material;
                    return material;
                }
            }
        }
        return loadDefaultMaterial();
    }

    ref<Technique> GLTF2Loader::Impl::loadTechnique(const string& id) {
        RETURN_IF_FOUND(_techniquesOld, id);

        if (_useDefaultMaterial) {
            return loadDefaultTechnique();
        }

        auto jTechniques = _json.find(TECHNIQUES);
        if (jTechniques != _json.end()) {
            auto jTechnique = jTechniques->find(id);
            if (jTechnique != jTechniques->end()) {

                auto tech = Technique::create();
                // TODO technique name?

                // program
                auto programId = jTechnique->value("program", "");
                if (!programId.empty()) {
                    tech->setEffect(loadProgram(programId));
                }

                // parameters
                auto jParams = jTechnique->find("parameters");
                if (jParams != jTechnique->end()) {
                    // attributes
                    auto jAttribs = jTechnique->find(ATTRIBUTES);
                    if (jAttribs != jTechnique->end()) {
                        for (auto attrib = jAttribs->begin(); attrib != jAttribs->end(); ++attrib) {
                            loadTechniqueAttribute(attrib.key(), attrib.value(), *jParams, tech);
                        }
                    }

                    // uniforms
                    auto jUniforms = jTechnique->find("uniforms");
                    if (jUniforms != jTechnique->end()) {
                        for (auto uniform = jUniforms->begin(); uniform != jUniforms->end(); ++uniform) {
                            loadTechniqueUniform(uniform.key(), uniform.value(), *jParams, tech);
                        }
                    }
                }
                // render states
                auto jStates = jTechnique->find("states");
                if (jStates != jTechnique->end()) {
                    auto jEnable = jStates->find("enable");
                    if (jEnable != jStates->end() && jEnable->is_array()) {
                        for (auto e : *jEnable) {
                            int state = e.get<int>();
                            if (state > 0) {
                                setState(state, tech->renderState());
                            }
                        }
                    }

                    auto jFunctions = jStates->find("functions");
                    if (jFunctions != jStates->end()) {
                        for (auto f = jFunctions->begin(); f != jFunctions->end(); ++f) {
                            setFunction(tech->renderState(), f.key(), f.value());
                        }
                    }
                }

                //_techniques[id] = tech; // TODO needs to be fixed
                return tech;
            }
        }
        return loadDefaultTechnique();
    }

    void GLTF2Loader::Impl::loadTechniqueAttribute(const string& glslName, const string& paramName, const json& parameters, ref<Technique> tech) {
        auto jParam = parameters.find(paramName);
        if (jParam != parameters.end()) {
            string semantic(jParam->value(SEMANTIC, ""));
            int type = jParam->value<int>(TYPE, 0);
            if (type != 0 && !semantic.empty()) {
                tech->setAttribute(glslName, toAttributeSemantic(semantic));
            }
        }
    }

    void GLTF2Loader::Impl::loadTechniqueUniform(const string& glslName, const string& paramName, const json& parameters, ref<Technique> tech) {
        auto jParam = parameters.find(paramName);
        if (jParam != parameters.end()) {
            auto jSemantic = jParam->find(SEMANTIC);
            //int type = jParam->value<int>(TYPE, 0);
            // TODO check type?
            if (jSemantic != jParam->end() && jSemantic->is_string()) {
                auto semanticValue = toSemantic(jSemantic->get_ref<const string&>());

                bool useSemantic = true;
                auto jNodeId = jParam->find("node");
                if (jNodeId != jParam->end() && jNodeId->is_string()) {
                    auto node = loadNode(jNodeId->get_ref<const string&>());
                    if (node) {
                        // TODO this is cheating
                        if (semanticValue == MaterialParameter::Semantic::MODELVIEW) {
                            auto f = [node](Effect& effect, const Uniform* uniform) {
                                effect.setValue(uniform, node->modelViewMatrix());
                            };
                            auto param = MaterialParameter::create(paramName);
                            param->setValue(f);
                            tech->setUniform(glslName, param);
                            useSemantic = false;
                        }
                    }
                }
                if (useSemantic) {
                    tech->setSemanticUniform(glslName, paramName, semanticValue);
                }
            }
            else {
                auto jValue = jParam->find("value");
                if (jValue != jParam->end()) {
                    const auto& value = *jValue;
                    if (value.is_number()) {
                        // TODO check the type. Don't assume float.
                        auto param = MaterialParameter::create(paramName);
                        param->setValue(value.get<float>());
                        tech->setUniform(glslName, param);
                    }
                    else if (value.is_array()) {
                        auto size = value.size();
                        if (size == 2) {
                            auto vec = glm::make_vec2<float>(&value.get<std::vector<float>>()[0]);
                            auto param = MaterialParameter::create(paramName);
                            param->setValue(vec);
                            tech->setUniform(glslName, param);
                        }
                        else if (size == 3) {
                            auto vec = glm::make_vec3<float>(&value.get<std::vector<float>>()[0]);
                            auto param = MaterialParameter::create(paramName);
                            param->setValue(vec);
                            tech->setUniform(glslName, param);
                        }
                        else if (size == 4) {
                            auto vec = glm::make_vec4<float>(&value.get<std::vector<float>>()[0]);
                            auto param = MaterialParameter::create(paramName);
                            param->setValue(vec);
                            tech->setUniform(glslName, param);
                        }
                    }
                }
                tech->setUniformName(glslName, paramName);
            }
        }
    }

    ref<Effect> GLTF2Loader::Impl::loadProgram(const string& id) {
        RETURN_IF_FOUND(_effectsOld, id);

        auto jPrograms = _json.find("programs");
        if (jPrograms != _json.end()) {
            auto jProgram = jPrograms->find(id);
            if (jProgram != jPrograms->end()) {

                auto fragShaderId = jProgram->value("fragmentShader", "");
                auto vertShaderId = jProgram->value("vertexShader", "");

                if (!fragShaderId.empty() && !vertShaderId.empty()) {
                    string fragSource;
                    string vertSource;
                    loadShaderSource(fragShaderId, fragSource);
                    loadShaderSource(vertShaderId, vertSource);
                    auto effect = Effect::createFromSource(vertSource, fragSource);
                    if (effect) {
                        _effectsOld[id] = effect;
                        return effect;
                    }
                    else {
                        loge("LOAD_GLTF_PROGRAM");
                    }
                }
            }
        }
        return nullptr;
    }

    ref<Texture> GLTF2Loader::Impl::loadTexture(const string& id) {
        RETURN_IF_FOUND(_texturesOld, id);

        const auto jTextures = _json.find("textures");
        if (jTextures != _json.end()) {
            const auto jTexture = jTextures->find(id);
            if (jTexture != jTextures->end()) {
                //auto format = jTexture->value<int>("format", DEFAULT_FORMAT);
                auto internalFormat = jTexture->value<int>("internalFormat", DEFAULT_FORMAT);
                auto target = jTexture->value<int>("target", DEFAULT_TARGET);
                //auto type = jTexture->value<int>(TYPE, DEFAULT_TEXEL_TYPE);

                const auto samplerId = jTexture->find("sampler");
                const auto imageId = jTexture->find("source");

                // TODO support more than just GL_TEXTURE_2D
                if (target == GL_TEXTURE_2D && samplerId != jTexture->end() && imageId != jTexture->end()
                    && samplerId->is_string() && imageId->is_string()) {
                    auto image = loadImage(imageId->get_ref<const string&>());
                    if (image) {
                        auto texture = Texture::create2D(image.get(), internalFormat, true);
                        if (texture) {
                            auto sampler = loadSampler(samplerId->get_ref<const string&>());
                            if (sampler) {
                                texture->setSampler(sampler);
                            }
                            else {
                                loge("LOAD_GLTF_SAMPLER ", samplerId->get_ref<const string&>().c_str());
                            }
                            _texturesOld[id] = texture;
                            return texture;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    ref<Sampler> GLTF2Loader::Impl::loadSampler(const string& id) {
        RETURN_IF_FOUND(_samplersOld, id);

        const auto jSamplers = _json.find("samplers");
        if (jSamplers != _json.end()) {
            const auto jSampler = jSamplers->find(id);
            if (jSampler != jSamplers->end()) {
                auto sampler = Sampler::create();
                auto magFilter = toMagFilterMode(jSampler->value<int>("magFilter", DEFAULT_MAG_FILTER));
                auto minFilter = toMinFilterMode(jSampler->value<int>("minFilter", DEFAULT_MIN_FILTER));
                auto wrapS = toWrapMode(jSampler->value<int>("wrapS", DEFAULT_WRAP_S));
                auto wrapT = toWrapMode(jSampler->value<int>("wrapT", DEFAULT_WRAP_T));
                // GLTF doesn't have WRAP_R
                sampler->setFilterMode(minFilter, magFilter);
                sampler->setWrapMode(wrapS, wrapT);
                return sampler;
            }
        }
        return nullptr;
    }

    ref<Image> GLTF2Loader::Impl::loadImage(const string& id) {
        RETURN_IF_FOUND(_imagesOld, id);
        const auto jImages = _json.find("images");
        if (jImages != _json.end()) {
            const auto jImage = jImages->find(id);
            if (jImage != jImages->end()) {
                // TODO is name needed?
                const auto jUri = jImage->find(URI);
                if (jUri != jImage->end() && jUri->is_string()) {
                    const auto& uri = jUri->get_ref<const string&>();
                    ref<Image> image;
                    if (startsWith(uri, DATA_IMAGE_BASE64)) {
                        auto index = uri.find_first_of(',');
                        if (index != string::npos) {
                            auto buffer = createBufferFromBase64(uri, index + 1);
                            image = Image::createFromFileMemory(&((*buffer)[0]), static_cast<int>(buffer->size()));
                        }
                    }
                    else {
                        auto path = uriToPath(uri);

                        auto start = std::chrono::system_clock::now();

                        image = Image::createFromFile(path.c_str(), false);

                        auto end = std::chrono::system_clock::now();
                        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                        auto clogWidth = std::clog.width();
                        std::clog.width(8);
                        std::clog << static_cast<double>(time.count());
                        std::clog.width(clogWidth);
                        std::clog << " ms to load " << path.c_str() << std::endl;
                    }
                    if (image) {
                        _imagesOld[id] = image;
                        return image;
                    }
                    // TODO load from base64 string
                }
            }
        }
        return nullptr;
    }

    ref<Material> GLTF2Loader::Impl::loadDefaultMaterial() {
        if (_defaultMaterial) {
            return _defaultMaterial;
        }
        auto material = Material::create();

        auto emission = MaterialParameter::create("emission");
        emission->setValue(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        material->addParam(emission);

        auto technique = loadDefaultTechnique();
        material->setTechnique(technique);
        return material;
    }

    ref<Technique> GLTF2Loader::Impl::loadDefaultTechnique() {
        if (_defaultTechnique) {
            return _defaultTechnique;
        }

        auto effect = Effect::createFromSource(DEFAULT_VERT_SHADER, DEFAULT_FRAG_SHADER);
        if (effect == nullptr) {
            loge("LOAD_DEFAULT_EFFECT");
            return nullptr;
        }
        auto tech = Technique::create();
        tech->setEffect(effect);

        tech->setAttribute("a_normal", AttributeSemantic::NORMAL);
        tech->setAttribute("a_position", AttributeSemantic::POSITION);

        tech->setUniformName("u_emission", "emission");

        tech->setSemanticUniform("u_modelViewMatrix", "modelViewMatrix", MaterialParameter::Semantic::MODELVIEW);
        tech->setSemanticUniform("u_normalMatrix", "normalMatrix", MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE);
        tech->setSemanticUniform("u_projectionMatrix", "projectionMatrix", MaterialParameter::Semantic::PROJECTION);

        auto& state = tech->renderState();
        state.setCulling(true);
        state.setDepthTest(true);

        _defaultTechnique = tech;
        return _defaultTechnique;
    }

    void GLTF2Loader::Impl::loadShaderSource(const string& id, string& destination) {
        auto jShaders = _json.find("shaders");
        if (jShaders != _json.end()) {
            auto jShader = jShaders->find(id);
            if (jShader != jShaders->end()) {
                auto jUri = jShader->find(URI);
                if (jUri != jShader->end() && jUri->is_string()) {
                    const auto& uri = jUri->get_ref<const string&>();
                    if (startsWith(uri, DATA_TEXT_BASE64)) {
                        base64Decode(uri, DATA_TEXT_BASE64.length(), destination);
                    }
                    else if (!uri.empty()) {
                        string path = uriToPath(uri);
                        readTextFile(path.c_str(), destination);
                    }
                }
            }
        }
    }

    ref<Material> GLTF2Loader::Impl::loadMaterialByName(const string& name) {
        auto jMaterials = _json.find(MATERIALS);
        if (jMaterials != _json.end()) {
            for (auto it = jMaterials->begin(); it != jMaterials->end(); ++it) {
                auto jName = it->find("name");
                if (jName != it->end()) {
                    if (name == jName->get_ref<const string&>()) {
                        return loadMaterial(it.key());
                    }
                }
            }
        }
        return nullptr;
    }

    string GLTF2Loader::Impl::uriToPath(const string& uri) const {
        // TODO handle absolute paths for uri
        return joinPath(_baseDir, uri);
    }

    void GLTF2Loader::Impl::useDefaultMaterial(bool value) {
        _useDefaultMaterial = value;
    }

    void GLTF2Loader::Impl::setAutoLoadMaterials(bool value) {
        _autoLoadMaterials = value;
    }

    void GLTF2Loader::Impl::loadTransform(const gltf2::Node& gNode, const ref<Node>& node) {
        std::array<float, 16> matrix;
        if (gNode.matrix(matrix.data())) {
            node->setLocalTransform(glm::make_mat4(matrix.data()));
        }
        else {
            glm::vec3 trans;
            glm::vec3 scale(1.0f);
            glm::quat rot;
            gNode.translation(glm::value_ptr(trans));
            gNode.scale(glm::value_ptr(scale));
            gNode.rotation(glm::value_ptr(rot));
            node->setLocalTransform(trans, rot, scale);
        }
    }

    void GLTF2Loader::Impl::loadTransform(const json& jNode, ref<Node> node) {
        auto matrix = jNode.find(MATRIX);
        if (matrix != jNode.end()) {
            if (matrix->size() == 16) {
                float m[16];
                float* p = m;
                for (const auto& f : *matrix) {
                    *p++ = f;
                }
                node->setLocalTransform(glm::make_mat4(m));
            }
        }

        auto translation = jNode.find(TRANSLATION);
        auto rotation = jNode.find(ROTATION);
        auto scale = jNode.find(SCALE);

        if (translation != jNode.end() && rotation != jNode.end() && scale != jNode.end()) {
            if (translation->size() == 3 && rotation->size() == 4 && scale->size() == 3) {
                auto t = translation->get<std::vector<float>>();
                auto r = rotation->get<std::vector<float>>();
                auto s = scale->get<std::vector<float>>();
                node->setLocalTransform(glm::make_vec3(&t[0]), glm::make_quat(&r[0]), glm::make_vec3(&s[0]));
                return;
            }
        }
    }

    ////////////////////////////////////////////////////////////////

    MeshPrimitive::Mode toMode(int mode) {
        // TODO is this actually necessary?
        switch (mode) {
        case 0: return MeshPrimitive::POINTS;
        case 1: return MeshPrimitive::LINES;
        case 2: return MeshPrimitive::LINE_LOOP;
        case 3: return MeshPrimitive::LINE_STRIP;
        case 4: return MeshPrimitive::TRIANGLES;
        case 5: return MeshPrimitive::TRIANGLE_STRIP;
        case 6: return MeshPrimitive::TRIANGLE_FAN;
        default:
            return MeshPrimitive::TRIANGLES;
        }
    }

    MaterialParameter::Semantic toSemantic(const string& semantic) {
        switch (semantic.length()) {
        case 4: if (semantic == "VIEW") return MaterialParameter::Semantic::VIEW;
            break;
        case 5:
            if (semantic == "LOCAL") return MaterialParameter::Semantic::LOCAL;
            if (semantic == "MODEL") return MaterialParameter::Semantic::MODEL;
            break;
        case 8: if (semantic == "VIEWPORT") return MaterialParameter::Semantic::VIEWPORT;
            break;
        case 9: if (semantic == "MODELVIEW") return MaterialParameter::Semantic::MODELVIEW;
            break;
        case 10: if (semantic == "PROJECTION") return MaterialParameter::Semantic::PROJECTION;
            break;
        case 11: if (semantic == "VIEWINVERSE") return MaterialParameter::Semantic::VIEWINVERSE;
            break;
        case 12: if (semantic == "MODELINVERSE") return MaterialParameter::Semantic::MODELINVERSE;
            break;
        case 16: if (semantic == "MODELVIEWINVERSE") return MaterialParameter::Semantic::MODELVIEWINVERSE;
            break;
        case 17: if (semantic == "PROJECTIONINVERSE") return MaterialParameter::Semantic::PROJECTIONINVERSE;
            break;
        case 19: if (semantic == "MODELVIEWPROJECTION") return MaterialParameter::Semantic::MODELVIEWPROJECTION;
            break;
        case 21: if (semantic == "MODELINVERSETRANSPOSE") return MaterialParameter::Semantic::MODELINVERSETRANSPOSE;
            break;
        case 25: if (semantic == "MODELVIEWINVERSETRANSPOSE") return MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE;
            break;
        case 26: if (semantic == "MODELVIEWPROJECTIONINVERSE") return MaterialParameter::Semantic::MODELVIEWPROJECTIONINVERSE;
            break;
        default: break;
        }
        return MaterialParameter::Semantic::NONE;
    }

    AttributeSemantic toAttributeSemantic(const string& semantic) {
        switch (semantic[0]) {
        case 'C': if (semantic == "COLOR") return AttributeSemantic::COLOR;
            break;
        case 'J':
            if (semantic == "JOINT") return AttributeSemantic::JOINT;
            if (semantic == "JOINTMATRIX") return AttributeSemantic::JOINTMATRIX;
            break;
        case 'N': if (semantic == "NORMAL") return AttributeSemantic::NORMAL;
            break;
        case 'P': if (semantic == "POSITION") return AttributeSemantic::POSITION;
            break;
        case 'W': if (semantic == "WEIGHT") return AttributeSemantic::WEIGHT;
            break;
        case 'T':
            if (semantic.length() >= 10 && semantic[1] == 'E') {
                int n = semantic[9] - '0';
                if (semantic.length() == 11) {
                    n = n * 10 + (semantic[10] - '0');
                }
                switch (n) {
                case 0: return AttributeSemantic::TEXCOORD_0;
                case 1: return AttributeSemantic::TEXCOORD_1;
                case 2: return AttributeSemantic::TEXCOORD_2;
                case 3: return AttributeSemantic::TEXCOORD_3;
                case 4: return AttributeSemantic::TEXCOORD_4;
                case 5: return AttributeSemantic::TEXCOORD_5;
                case 6: return AttributeSemantic::TEXCOORD_6;
                case 7: return AttributeSemantic::TEXCOORD_7;
                case 8: return AttributeSemantic::TEXCOORD_8;
                case 9: return AttributeSemantic::TEXCOORD_9;
                case 10: return AttributeSemantic::TEXCOORD_10;
                case 11: return AttributeSemantic::TEXCOORD_11;
                case 12: return AttributeSemantic::TEXCOORD_12;
                case 13: return AttributeSemantic::TEXCOORD_13;
                case 14: return AttributeSemantic::TEXCOORD_14;
                case 15: return AttributeSemantic::TEXCOORD_15;
                case 16: return AttributeSemantic::TEXCOORD_16;
                case 17: return AttributeSemantic::TEXCOORD_17;
                case 18: return AttributeSemantic::TEXCOORD_18;
                case 19: return AttributeSemantic::TEXCOORD_19;
                case 20: return AttributeSemantic::TEXCOORD_20;
                case 21: return AttributeSemantic::TEXCOORD_21;
                case 22: return AttributeSemantic::TEXCOORD_22;
                case 23: return AttributeSemantic::TEXCOORD_23;
                case 24: return AttributeSemantic::TEXCOORD_24;
                case 25: return AttributeSemantic::TEXCOORD_25;
                case 26: return AttributeSemantic::TEXCOORD_26;
                case 27: return AttributeSemantic::TEXCOORD_27;
                case 28: return AttributeSemantic::TEXCOORD_28;
                case 29: return AttributeSemantic::TEXCOORD_29;
                case 30: return AttributeSemantic::TEXCOORD_30;
                case 31: return AttributeSemantic::TEXCOORD_31;
                default: break;
                }
            }
            break;
        default:
            break;
        }
        throw std::domain_error("Invalid attribute semantic type");
    }

    Sampler::Wrap toWrapMode(int wrap) {
        switch (wrap) {
        case GL_REPEAT: return Sampler::Wrap::REPEAT;
        case GL_CLAMP_TO_EDGE: return Sampler::Wrap::CLAMP_TO_EDGE;
        case GL_MIRRORED_REPEAT: return Sampler::Wrap::MIRRORED_REPEAT;
        default:
            throw std::domain_error("Invalid wrap mode");
        }
    }

    Sampler::MinFilter toMinFilterMode(int filter) {
        switch (filter) {
        case GL_NEAREST: return Sampler::MinFilter::NEAREST;
        case GL_LINEAR: return Sampler::MinFilter::LINEAR;
        case GL_NEAREST_MIPMAP_NEAREST: return Sampler::MinFilter::NEAREST_MIPMAP_NEAREST;
        case GL_LINEAR_MIPMAP_NEAREST: return Sampler::MinFilter::LINEAR_MIPMAP_NEAREST;
        case GL_NEAREST_MIPMAP_LINEAR: return Sampler::MinFilter::NEAREST_MIPMAP_LINEAR;
        case GL_LINEAR_MIPMAP_LINEAR: return Sampler::MinFilter::LINEAR_MIPMAP_LINEAR;
        default:
            throw std::domain_error("Invalid filter mode");
        }
    }

    Sampler::MagFilter toMagFilterMode(int filter) {
        switch (filter) {
        case GL_NEAREST: return Sampler::MagFilter::NEAREST;
        case GL_LINEAR: return Sampler::MagFilter::LINEAR;
        default:
            throw std::domain_error("Invalid filter mode");
        }
    }

    GLint numComponentsOfType(const string& type) {
        if (type.length() == 4) {
            char c0 = type[0];
            char c3 = type[3];
            if (c0 == 'V') {
                switch (c3) {
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                }
            }
            else if (c0 == 'M') {
                switch (c3) {
                case '2': return 4;
                case '3': return 9;
                case '4': return 16;
                }
            }
        }
        return 1;
    }

    void setState(int state, RenderState& block) {
        switch (state) {
        case GL_BLEND:
            block.setBlend(true);
            break;
        case GL_CULL_FACE:
            block.setCulling(true);
            break;
        case GL_DEPTH_TEST:
            block.setDepthTest(true);
            break;
        case GL_POLYGON_OFFSET_FILL:
            block.setPolygonOffsetFill(true);
            break;
        case GL_PRIMITIVE_RESTART_FIXED_INDEX:
            block.setPrimitiveRestartFixedIndex(true);
            break;
        case GL_RASTERIZER_DISCARD:
            block.setRasterizerDiscard(true);
            break;
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
            block.setSampleAlphaToCoverage(true);
            break;
        case GL_SAMPLE_COVERAGE:
            block.setSampleCoverage(true);
            break;
        case GL_SAMPLE_MASK:
            block.setSampleMask(true);
            break;
        case GL_SCISSOR_TEST:
            block.setScissorTest(true);
            break;
        case GL_STENCIL_TEST:
            block.setStencilTest(true);
            break;
        default:
            logw("Unknown enable state");
            break;
        }
    }

    void setFunction(RenderState& renderState, const string& key, const json& value) {
        if (!value.is_array()) {
            return;
        }
        auto size = value.size();
        switch (key.length()) {
        case 7:
            //scissor
            if (size == 4) {
                // TODO mistake in the GLTF spec?
                renderState.setScissor(value[0].get<int>(), value[1].get<int>(), value[2].get<int>(), value[3].get<int>());
            }
            break;
        case 8:
            //cullFace
            if (size == 1) {
                renderState.setCullFace(value[0].get<int>());
            }
            break;
        case 9:
            switch (key[0]) {
            case 'c':
                //colorMask
                if (size == 4) {
                    renderState.setColorMask(value[0].get<bool>(), value[1].get<bool>(), value[2].get<bool>(), value[3].get<bool>());
                }
                break;
            case 'd':
                //depthFunc
                //depthMask
                if (size == 1) {
                    if (key == "depthFunc") {
                        renderState.setDepthFunc(value[0].get<int>());
                    }
                    else if (key == "depthMask") {
                        renderState.setDepthMask(value[0].get<bool>());
                    }
                }
                break;
            case 'f':
                //frontFace
                if (size == 1) {
                    renderState.setFrontFace(value[0].get<int>());
                }
                break;
            case 'l':
                //lineWidth
                if (size == 1) {
                    renderState.setLineWidth(value[0].get<float>());
                }
                break;
            default:
                break;
            }
            break;
        case 10:
            //blendColor
            //depthRange
            if (size == 4 && key == "blendColor") {
                renderState.setBlendColor(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>());
            }
            else if (size == 2 && key == "depthRange") {
                renderState.setDepthRange(value[0].get<float>(), value[1].get<float>());
            }
            break;
        case 13:
            //polygonOffset
            if (size == 2) {
                renderState.setPolygonOffset(value[0].get<float>(), value[0].get<float>());
            }
            break;
        case 17:
            //blendFuncSeparate
            if (size == 4) {
                renderState.setBlendFuncSeparate(value[0].get<int>(), value[1].get<int>(), value[2].get<int>(), value[3].get<int>());
            }
            break;
        case 21:
            //blendEquationSeparate
            if (size == 2) {
                renderState.setBlendEquationSeparate(value[0].get<int>(), value[0].get<int>());
            }
            break;
        default:
            logw("Unknown state function", key.c_str());
            break;
        }
    }

    void base64Decode(const string& text, size_t offset, string& destination) {
        base64::decoder d;
        std::istringstream in(text);
        in.seekg(offset);
        std::ostringstream out;
        d.decode(in, out);
        destination = out.str();
    }

    void base64Decode(std::istream& in, std::vector<ubyte>& out) {
        base64::decoder decoder;
        base64::base64_init_decodestate(&decoder._state);
        std::array<char, BUFFERSIZE> code;
        std::array<char, BUFFERSIZE> plaintext;
        int codelength;
        do {
            in.read(code.data(), code.size());
            codelength = static_cast<int>(in.gcount());
            int plainlength = decoder.decode(code.data(), codelength, plaintext.data());
            out.insert(out.end(), plaintext.begin(), plaintext.begin() + plainlength);
        } while (in.good() && codelength > 0);
        base64_init_decodestate(&decoder._state);
    }

    shared_ptr<std::vector<ubyte>> createBufferFromBase64(const string& text, size_t offset) {
        std::istringstream in(text);
        in.seekg(offset);
        auto buffer = std::make_shared<std::vector<ubyte>>();
        buffer->reserve(text.length() * 3 >> 2); // the data will be roughly 75% the size of the base64 text
        base64Decode(in, *buffer);
        return buffer;
    }
}
