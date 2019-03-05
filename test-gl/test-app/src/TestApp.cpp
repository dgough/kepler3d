#include "TestApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Platform.hpp>
#include <App.hpp>
#include <GLTF2Loader.hpp>
#include <Node.hpp>
#include <Transform.hpp>
#include <Camera.hpp>
#include <Effect.hpp>
#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>
#include <Scene.hpp>
#include <Mesh.hpp>
#include <MeshPrimitive.hpp>
#include <Material.hpp>
#include <Technique.hpp>
#include <VertexAttributeAccessor.hpp>
#include <IndexAccessor.hpp>
#include <MeshRenderer.hpp>
#include <FirstPersonController.hpp>
#include <AxisCompass.hpp>
#include <Image.hpp>
#include <Texture.hpp>
#include <Sampler.hpp>
#include <RenderState.hpp>
#include <Logging.hpp>
#include <StringUtils.hpp>
#include <MeshUtils.hpp>
#include <Performance.hpp>
#include <ColorMath.hpp>

#include <iostream>

namespace kepler {
namespace gl {

#define SAMPLES_BASE "../../../glTF-Sample-Models/2.0/"

static constexpr char* BOX_PATH = SAMPLES_BASE "Box/glTF/Box.gltf";
static constexpr char* BOX_TEXTURED_PATH = SAMPLES_BASE "BoxTextured/glTF/BoxTextured.gltf";
static constexpr char* BOX_INTERLEAVED_PATH = SAMPLES_BASE "BoxInterleaved/glTF/BoxInterleaved.gltf";
static constexpr char* TRUCK_PATH = SAMPLES_BASE "CesiumMilkTruck/glTF/CesiumMilkTruck.gltf";
static constexpr char* DUCK_PATH = SAMPLES_BASE "Duck/glTF/Duck.gltf";

static float g_deltaTime = 0.0f;

static bool g_use_vsync = false;
static bool g_showText = true;

static std::string g_fps_str;
static std::string g_frame_time_str;
static int g_frame_count = 0;

static void renderAll(Node* node) {
    auto renderer = node->component<DrawableComponent>();
    if (renderer) {
        renderer->draw();
    }
}
static void changeBoxColor(const Scene& scene);
static shared_ptr<Mesh> createCubeMesh();
static void printCursor() {
    double x, y;
    app()->cursorPosition(&x, &y);
    std::clog << "x=" << x << " y=" << y << std::endl;
}

TestApp::TestApp() {
}

void TestApp::start() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    app()->setSwapInterval(g_use_vsync ? 1 : 0);
    loadScenes();

    float width = static_cast<float>(app()->width());
    float height = static_cast<float>(app()->height());
    _firstPerson = std::make_unique<FirstPersonController>(45.0f, width, height, 0.1f, 200.0f);
    _firstPerson->setInvertY(false);
    _firstPerson->moveBackward(5.0f);
    if (_scene) {
        _scene->addNode(_firstPerson->rootNode());
        _scene->setActiveCamera(_firstPerson->camera());
        _compass = std::make_unique<AxisCompass>(_scene.get());
    }

    GLTF2Loader::printTotalTime();

    _font = BmpFont::createFromFile("res/fonts/arial-32.fnt");
    if (!_font) {
        loge("ERROR::failed to load font");
        app()->setShouldClose(true);
        return;
    }
    if (_scene) {
        changeBoxColor(*_scene);
    }
}

void TestApp::update() {
    doMovement();
}

void TestApp::render() {
    ++g_frame_count;
    g_deltaTime = static_cast<float>(deltaTime());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene) {
        static auto truck = _scene->findFirstNodeByName("Cesium_Milk_Truck");
        if (truck) {
            truck->rotateY(g_deltaTime * PI_OVER_2);
        }
        _scene->visit(renderAll);
    }

    auto fps = 1.0f / g_deltaTime;
    if (fps < 55.f) {
        //std::cout << fps << std::endl;
    }

    static float displayFrameTime = 0;
    displayFrameTime += g_deltaTime;
    if (displayFrameTime > 0.5f) {
        displayFrameTime -= 0.5f;
        g_frame_time_str = std::to_string(g_deltaTime * 1000.0);
        g_fps_str = std::to_string(1.0f / g_deltaTime);
    }

    if (g_showText) {
        drawText();
    }
}

void TestApp::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        app()->setShouldClose(true);
    }
    if (key == KEY_Q && action == PRESS && (mods & MOD_CTRL) != 0) {
        app()->setShouldClose(true);
    }
    if (action == PRESS) {
        switch (key) {
        case KEY_H:
            g_showText = !g_showText;
            break;
        case KEY_T:
            //testing();
            break;
        case KEY_M:
            app()->setCursorVisibility(!app()->isCursorVisible());
            break;
        case KEY_I:
            _firstPerson->setInvertY(!_firstPerson->isInvertedY());
            break;
        case KEY_V:
            g_use_vsync = !g_use_vsync;
            app()->setSwapInterval(g_use_vsync ? 1 : 0);
            std::clog << (g_use_vsync ? "enable" : "disable") << " VSYNC" << std::endl;
            break;
        case KEY_K:
            changeBoxColor(*_scene);
            break;
        case KEY_U:
            printCursor();
            break;
        case KEY_1:
            ProfileBlock::printTime(0);
            break;
        case KEY_2:
            ProfileBlock::printTime(1);
            break;
        case KEY_3:
            ProfileBlock::printTime(2);
            break;
        default:
            break;
        }
    }
}

void TestApp::mouseEvent(double xpos, double ypos) {
    static bool firstMouse = true;
    if (app()->isCursorVisible() && !app()->getMouseButton(LEFT_MOUSE)) {
        firstMouse = true;
        return;
    }
    float width = static_cast<float>(app()->width());
    float height = static_cast<float>(app()->height());
    static GLfloat lastX = width * 0.5f;
    static GLfloat lastY = height * 0.5f;
    if (firstMouse) {
        lastX = (GLfloat)xpos;
        lastY = (GLfloat)ypos;
        firstMouse = false;
    }

    GLfloat xoffset = (GLfloat)xpos - lastX;
    GLfloat yoffset = lastY - (GLfloat)ypos;
    lastX = (GLfloat)xpos;
    lastY = (GLfloat)ypos;

    GLfloat sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    _firstPerson->rotate(xoffset, yoffset);
}

void TestApp::mouseButtonEvent(int button, int action, int mods) {
}

void TestApp::scrollEvent(double xoffset, double yoffset) {
    _firstPerson->moveForward(static_cast<float>(yoffset));
}

void TestApp::dropEvent(int count, const char** paths) {
    for (int i = 0; i < count; ++i) {
        const char* path = paths[i];
        if (endsWith(path, ".gltf")) {
            std::clog << "Load: " << path << std::endl;
            loadGLTF(paths[i]);
        }
    }
}

void TestApp::doMovement() {
    auto app = kepler::app();
    GLfloat cameraSpeed = 5.0f * g_deltaTime;

    if (app->getKey(KEY_LEFT_SHIFT)) {
        cameraSpeed *= 3.0f;
    }
    GLfloat upSpeed = cameraSpeed;
    vec3 move;
    if (app->getKey(KEY_W)) {
        move.z = 1;
    }
    else if (app->getKey(KEY_S)) {
        move.z = -1;
    }
    if (app->getKey(KEY_A)) {
        move.x = -1;
    }
    if (app->getKey(KEY_D)) {
        move.x = 1;
    }
    if (move.x != 0 || move.y != 0 || move.z != 0) {
        move = glm::normalize(move);
    }
    move *= cameraSpeed;

    _firstPerson->moveForward(move.z);
    _firstPerson->moveRight(move.x);

    if (app->getKey(KEY_E)) {
        _firstPerson->moveUp(upSpeed);
    }
    if (app->getKey(KEY_Q)) {
        _firstPerson->moveDown(upSpeed);
    }
}

void TestApp::drawText() {
    float y = 0.0f;
    auto lineHeight = _font->lineHeight();

    float size = static_cast<float>(_font->size());

    _font->drawText(g_frame_time_str.c_str(), 0.f, y, vec3(1, 0, 0));
    _font->drawText(g_fps_str.c_str(), 0.f, y += size, vec3(0, 0.8f, 0.2f));
    if (app()->isCursorVisible()) {
        _font->drawText("m - activate mouse look", 0.f, y += size, vec3(1, 0.41f, 0));
    }
    else {
        _font->drawText("m - disable mouse look", 0.f, y += size, vec3(1, 0.41f, 0));
    }

    static vec3 hColor = rgbToVec3(0xFFBC42);
    _font->drawText("k - change box color", 0.f, y += size, hColor);
    //_font->drawText("The quick brown fox", x, y += size);
    //_font->drawText("AA�AA", x, y += sizevec3c3(1, 0, 0));
    //_font->drawText(_font->getFace().c_str(), x, y += size, vec3(0, 1, 1));

    static vec3 clickColor(1.0f, 0.7686f, 0.4f);
    if (app()->getMouseButton(LEFT_MOUSE)) {
        _font->drawText("LEFT PRESS", 0, app()->height() - size, clickColor);
    }
    else {
        _font->drawText("LEFT RELEASE", 0, app()->height() - size, clickColor);
    }
}

void TestApp::loadGLTF(const char* path) {
    GLTF2Loader gltf;
    //gltf.useDefaultMaterial(true);
    auto scene = gltf.loadSceneFromFile(path);
    if (_scene) {
        _scene->moveNodesFrom(scene);
    }
    else {
        _scene = scene;
    }
}

void TestApp::loadScenes() {
    auto scene = loadDuckScene();

    //scene->getChildAt(0)->translateY(1);
    //scene = gltf.loadSceneFromFile("res/glTF/2CylinderEngine.gltf");
    if (scene) {
        _scene = scene;
        GLTF2Loader cyLoader;
        //cyLoader.useDefaultMaterial(true);
        shared_ptr<Scene> cylinder = nullptr;// cyLoader.loadSceneFromFile("res/glTF/2CylinderEngine.gltf");
        if (scene && cylinder) {
            for (const auto& n : cylinder->children()) {
                if (n->localTransform().scale().x >= 0.9f) {
                    n->scale(0.003f);
                }
            }
            scene->moveNodesFrom(cylinder);
        }

        GLTF2Loader truckLoader;
        //truckLoader.useDefaultMaterial(true);
        auto truckScene = truckLoader.loadSceneFromFile(TRUCK_PATH);
        auto truck = truckScene->childAt(0);
        truck->setName("Cesium_Milk_Truck");
        scene->moveNodesFrom(truckScene);
        if (truck) {
            truck->translate(-2.5, -0.5, 0);
            truck->rotateY(-PI / 4);
            truck->scale(0.5f);
        }

        scene->moveNodesFrom(GLTF2Loader().loadSceneFromFile(BOX_PATH));
        scene->lastChild()->childAt(0)->setName("Box");
        scene->lastChild()->setTranslation(vec3(0, -2, 0));
        scene->moveNodesFrom(GLTF2Loader().loadSceneFromFile(BOX_INTERLEAVED_PATH));
        scene->lastChild()->setTranslation(vec3(2, -2, 0));
        scene->moveNodesFrom(GLTF2Loader().loadSceneFromFile(BOX_TEXTURED_PATH));
        scene->lastChild()->setTranslation(vec3(2, 0, 0));
    }

    auto cube = createCubeMesh();
    if (cube && _scene) {
        auto n = _scene->createChild("cube");
        n->addComponent(MeshRenderer::create(cube));
        n->translate(-2.f, -2.f, 0.f);
    }
}

shared_ptr<Scene> TestApp::loadDuckScene() {
    GLTF2Loader gltf;
    auto scene = gltf.loadSceneFromFile(DUCK_PATH);
    if (scene && scene->childCount() > 0) {
        scene->childAt(0)->translateY(1);
        if (auto lightNode = scene->findFirstNodeByName("directionalLight1")) {
            lightNode->rotateY(glm::pi<float>());
        }
    }
    return scene;
}

void changeBoxColor(const Scene& scene) {
    static vec4 color(0.8, 0, 0, 1);
    color = vec4(color.z, color.x, color.y, color.w);

    auto redBox = scene.findFirstNodeByName("Box");
    if (redBox) {
        if (auto mesh = redBox->component<MeshRenderer>()->mesh()) {
            if (auto prim = mesh->primitiveAt(0)) {
                if (auto mat = prim->material()) {
                    if (auto tech = mat->technique()) {
                        if (auto param = tech->findValueParameter("baseColorFactor")) {
                            param->setValue(color);
                        }
                    }
                }
            }
        }
    }
}

shared_ptr<Mesh> createCubeMesh() {
    //auto prim = createLitCubePrimitive();
    //if (prim) {
    //    GLTF2Loader gltf;
    //    gltf.load(BOX_PATH);
    //    auto material = gltf.findMaterialById("Effect-Red");
    //    if (auto param = material->param("diffuse")) {
    //        param->setValue(vec4(1.f, 0.5f, 0.f, 1.0f));
    //    }
    //    prim->setMaterial(material);
    //    auto mesh = Mesh::create();
    //    mesh->addMeshPrimitive(prim);
    //    return mesh;
    //}
    return nullptr;
}
}
}
