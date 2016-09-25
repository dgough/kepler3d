#include "SceneTest.hpp"
#include "MainMenu.hpp"

#include <BaseGL.hpp>
#include <App.hpp>
#include <GLTFLoader.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <MeshRenderer.hpp>
#include <StringUtils.hpp>
#include <BmpFont.hpp>

#include <iostream>

using namespace kepler;
using glm::vec3;

static constexpr char* GLTF_PATH =
//"C:/dev/github/glTF/sampleModels/Buggy/glTF/Buggy.gltf";
//"C:/dev/github/glTF/sampleModels/GearboxAssy/glTF/GearboxAssy.gltf";
//"res/glTF/duck/Duck.gltf";
"res/glTF/CesiumMilkTruck/CesiumMilkTruck.gltf";
//"res/glTF/BoxWithoutIndices/BoxWithoutIndices.gltf";
//"res/glTF/ReciprocatingSaw/ReciprocatingSaw.gltf";

static std::string g_text;

SceneTest::SceneTest() : _moveCamera(false) {
}

SceneTest::~SceneTest() {
}

void SceneTest::start() {
    //glClearColor(0.16f, 0.3f, 0.5f, 1.0f);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    loadSceneFromFile(GLTF_PATH);
    if (_scene) {
        if (auto node = _scene->findFirstNodeByName("pro12-18s")) {
            node->editLocalTransform().loadIdentity();
        }
    }

    _font = BmpFont::createFromFile("res/fonts/arial-32.fnt");
}

void SceneTest::update() {
}

void SceneTest::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene) {
        _scene->visit([](Node* node) {
            if (auto renderer = node->drawable()) {
                renderer->draw();
            }
        });
    }

    if (_font) {
        _font->drawText(g_text.c_str(), 0.f, 0.f);
    }
    _compass.draw();
}

void SceneTest::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        MainMenu::gotoMainMenu();
    }
    if (action == PRESS) {
        switch (key) {
        case KEY_F:
            _orbitCamera.setZoom(10.f);
            break;
        default:
            break;
        }
    }

    // TODO press f to focus
}

void SceneTest::mouseEvent(double xpos, double ypos) {
    if (_moveCamera && app()->getMouseButton(LEFT_MOUSE)) {
        _orbitCamera.move(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void SceneTest::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
        double x, y;
        app()->cursorPosition(&x, &y);
        _orbitCamera.start(static_cast<float>(x), static_cast<float>(y));
        _moveCamera = true;
    }
}

void SceneTest::scrollEvent(double xoffset, double yoffset) {
    _orbitCamera.zoomOut(static_cast<float>(-yoffset));
}

void SceneTest::dropEvent(int count, const char** paths) {
    // Load the first GLTF file found. Ignore directories.
    for (int i = 0; i < count; ++i) {
        const char* path = paths[i];
        if (endsWith(path, ".gltf")) {
            std::clog << "Load: " << path << std::endl;
            loadSceneFromFile(paths[i]);
            return;
        }
    }
}

void SceneTest::loadSceneFromFile(const char* path) {
    if (path == nullptr || *path == '\0') {
        std::clog << "Invalid path" << std::endl;
        return;
    }
    _orbitCamera.detach();
    _scene.reset();

    GLTFLoader loader;
    loader.setCameraAspectRatio(app()->aspectRatio());
    _scene = loader.loadSceneFromFile(path);

    if (_scene) {
        _orbitCamera.attach(_scene);
        _compass.setScene(_scene);
    }
    g_text.assign(path);
}
