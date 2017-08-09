#include "Gltf2Test.hpp"
#include "MainMenu.hpp"

#include <BaseGL.hpp>
#include <App.hpp>
#include <GLTF2Loader.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <MeshRenderer.hpp>
#include <StringUtils.hpp>
#include <BmpFont.hpp>
#include <Performance.hpp>
#include <Mesh.hpp>
#include <Scene.hpp>

#include <iostream>
#include <algorithm>

using namespace kepler;
using glm::vec3;

static std::vector<const char*> g_paths{
    "../../glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
    "../../glTF-Sample-Models/2.0/Duck/glTF/Duck.gltf",
    "../../glTF-Sample-Models/2.0/Lantern/glTF/Lantern.gltf",
    "../../glTF-Sample-Models/2.0/SciFiHelmet/glTF/SciFiHelmet.gltf",
    "../../glTF-Sample-Models/2.0/Box/glTF/Box.gltf",
    "../../glTF-Sample-Models/2.0/BoxAnimated/glTF/BoxAnimated.gltf",
    "../../glTF-Sample-Models/2.0/AnimatedCube/glTF/AnimatedCube.gltf",
    "../../glTF-Sample-Models/2.0/AnimatedMorphCube/glTF/AnimatedMorphCube.gltf",
    "../../glTF-Sample-Models/2.0/AnimatedMorphSphere/glTF/AnimatedMorphSphere.gltf",
    "../../glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf",
    "../../glTF-Sample-Models/2.0/BarramundiFish/glTF/BarramundiFish.gltf",
    "../../glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf",
    "../../glTF-Sample-Models/2.0/BoxInterleaved/glTF/BoxInterleaved.gltf",
    "../../glTF-Sample-Models/2.0/BoxTextured/glTF/BoxTextured.gltf",
    "../../glTF-Sample-Models/2.0/BrainStem/glTF/BrainStem.gltf",
    "../../glTF-Sample-Models/2.0/Buggy/glTF/Buggy.gltf",
    "../../glTF-Sample-Models/2.0/Cameras/glTF/Cameras.gltf",
    "../../glTF-Sample-Models/2.0/CesiumMan/glTF/CesiumMan.gltf",
    "../../glTF-Sample-Models/2.0/Corset/glTF/Corset.gltf",
    "../../glTF-Sample-Models/2.0/Cube/glTF/Cube.gltf",
    "../../glTF-Sample-Models/2.0/GearboxAssy/glTF/GearboxAssy.gltf",
    "../../glTF-Sample-Models/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf",
    "../../glTF-Sample-Models/2.0/Monster/glTF/Monster.gltf",
    "../../glTF-Sample-Models/2.0/NormalTangentTest/glTF/NormalTangentTest.gltf",
    "../../glTF-Sample-Models/2.0/ReciprocatingSaw/glTF/ReciprocatingSaw.gltf",
    "../../glTF-Sample-Models/2.0/RiggedFigure/glTF/RiggedFigure.gltf",
    "../../glTF-Sample-Models/2.0/RiggedSimple/glTF/RiggedSimple.gltf",
    "../../glTF-Sample-Models/2.0/SimpleMeshes/glTF/SimpleMeshes.gltf",
    "../../glTF-Sample-Models/2.0/SmilingFace/glTF/SmilingFace.gltf",
    "../../glTF-Sample-Models/2.0/Suzanne/glTF/Suzanne.gltf",
    "../../glTF-Sample-Models/2.0/TextureSettingsTest/glTF/TextureSettingsTest.gltf",
    "../../glTF-Sample-Models/2.0/Triangle/glTF/Triangle.gltf",
    "../../glTF-Sample-Models/2.0/TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf",
    "../../glTF-Sample-Models/2.0/AnimatedTriangle/glTF/AnimatedTriangle.gltf",
    "../../glTF-Sample-Models/2.0/TwoSidedPlane/glTF/TwoSidedPlane.gltf",
    "../../glTF-Sample-Models/2.0/WalkingLady/glTF/WalkingLady.gltf",
    "../../glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf",
    "../../glTF-Sample-Models/2.0/VC/glTF/VC.gltf",
};
static size_t g_pathIndex = 0;

static std::string g_text;

static const char* nextPath() {
    g_pathIndex = (g_pathIndex + 1) % g_paths.size();
    return g_paths[g_pathIndex];
}

static const char* prevPath() {
    if (g_pathIndex == 0) {
        g_pathIndex = g_paths.size() - 1;
    }
    else {
        g_pathIndex = (g_pathIndex - 1) % g_paths.size();
    }
    return g_paths[g_pathIndex];
}

static const char* getCurrentPath() {
    return g_paths[g_pathIndex];
}

Gltf2Test::Gltf2Test() : _moveCamera(false) {
}

void Gltf2Test::start() {
    g_pathIndex = 0;
    //glClearColor(0.16f, 0.3f, 0.5f, 1.0f);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    loadSceneFromFile(getCurrentPath());
    if (_scene) {
        if (auto node = _scene->findFirstNodeByName("pro12-18s")) {
            node->editLocalTransform().loadIdentity();
        }
    }
    _font = BmpFont::createFromFile("res/fonts/arial-32.fnt");
}

void Gltf2Test::update() {
}

void Gltf2Test::render() {
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
}

void Gltf2Test::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        MainMenu::gotoMainMenu();
    }
    if (action == PRESS) {
        switch (key) {
        case KEY_F:
            focus();
            break;
        case KEY_N:
            loadNextPath();
            break;
        case KEY_P:
            loadPrevPath();
            break;
        case KEY_F4:
            if (mods & MOD_CTRL) {
                MainMenu::gotoMainMenu();
            }
            break;
        default:
            break;
        }
    }
}

void Gltf2Test::mouseEvent(double xpos, double ypos) {
    if (_moveCamera && app()->getMouseButton(LEFT_MOUSE)) {
        _orbitCamera.move(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void Gltf2Test::mouseButtonEvent(int button, int action, int mods) {
    if (action == PRESS) {
        if (button == LEFT_MOUSE) {
            double x, y;
            app()->cursorPosition(&x, &y);
            _orbitCamera.start(static_cast<float>(x), static_cast<float>(y));
            _moveCamera = true;
        }
        else if (button == FORWARD_MOUSE) {
            loadNextPath();
        }
        else if (button == BACK_MOUSE) {
            loadPrevPath();
        }
    }
}

void Gltf2Test::scrollEvent(double xoffset, double yoffset) {
    _orbitCamera.zoomOut(static_cast<float>(-yoffset));
}

void Gltf2Test::dropEvent(int count, const char** paths) {
    // Load the first GLTF file found. Ignore directories.
    for (int i = 0; i < count; ++i) {
        const char* path = paths[i];
        if (endsWith(path, ".gltf") || endsWith(path, ".glb")) {
            std::clog << "Load: " << path << std::endl;
            loadSceneFromFile(paths[i]);
            return;
        }
    }
}

void Gltf2Test::focus() {
    _orbitCamera.setZoom(10.0f);
}

void Gltf2Test::loadSceneFromFile(const char* path) {
    if (path == nullptr || *path == '\0') {
        std::clog << "Invalid path" << std::endl;
        return;
    }
    _orbitCamera.detach();
    _scene.reset();

    GLTF2Loader loader;
    loader.setCameraAspectRatio(app()->aspectRatio());
    _scene = loader.loadSceneFromFile(path);

    if (_scene) {
        _orbitCamera.attach(_scene.get());
        _scene->addNode(_compass.node());
    }
    g_text.assign(path);
    calcBoundingBox();
}

void Gltf2Test::loadNextPath() {
    loadSceneFromFile(nextPath());
}

void Gltf2Test::loadPrevPath() {
    loadSceneFromFile(prevPath());
}

void Gltf2Test::calcBoundingBox() {
    _box = BoundingBox();
    if (_scene) {
        _scene->visit([this](Node* node) {
            if (auto meshRenderer = node->component<MeshRenderer>()) {
                if (auto mesh = meshRenderer->mesh()) {
                    const auto& box = mesh->boundingBox();
                    if (_box.empty()) {
                        _box = box;
                    }
                    else {
                        _box.merge(box);
                    }
                }
            }
        });
    }
}
