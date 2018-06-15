#include "Gltf2Test.hpp"
#include "MainMenu.hpp"

#include <OpenGL.hpp>
#include <App.hpp>
#include <GLTF2Loader.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <MeshRenderer.hpp>
#include <StringUtils.hpp>
#include <BmpFont.hpp>
#include <Performance.hpp>
#include <Mesh.hpp>
#include <MeshPrimitive.hpp>
#include <MeshUtils.hpp>
#include <Scene.hpp>
#include <Technique.hpp>
#include <Material.hpp>

#include <iostream>
#include <algorithm>

namespace kepler {
namespace gl {

using glm::vec3;

#define SAMPLES_BASE "../../../glTF-Sample-Models/2.0/"

static std::vector<const char*> g_paths{
    SAMPLES_BASE "CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
    SAMPLES_BASE "Lantern/glTF/Lantern.gltf",
    SAMPLES_BASE "Duck/glTF/Duck.gltf",
    SAMPLES_BASE "SciFiHelmet/glTF/SciFiHelmet.gltf",
    SAMPLES_BASE "Box/glTF/Box.gltf",
    SAMPLES_BASE "BoxAnimated/glTF/BoxAnimated.gltf",
    SAMPLES_BASE "AnimatedCube/glTF/AnimatedCube.gltf",
    SAMPLES_BASE "AnimatedMorphCube/glTF/AnimatedMorphCube.gltf",
    SAMPLES_BASE "AnimatedMorphSphere/glTF/AnimatedMorphSphere.gltf",
    SAMPLES_BASE "Avocado/glTF/Avocado.gltf",
    SAMPLES_BASE "BarramundiFish/glTF/BarramundiFish.gltf",
    SAMPLES_BASE "BoomBox/glTF/BoomBox.gltf",
    SAMPLES_BASE "BoxInterleaved/glTF/BoxInterleaved.gltf",
    SAMPLES_BASE "BoxTextured/glTF/BoxTextured.gltf",
    SAMPLES_BASE "BrainStem/glTF/BrainStem.gltf",
    SAMPLES_BASE "Buggy/glTF/Buggy.gltf",
    SAMPLES_BASE "Cameras/glTF/Cameras.gltf",
    SAMPLES_BASE "CesiumMan/glTF/CesiumMan.gltf",
    SAMPLES_BASE "Corset/glTF/Corset.gltf",
    SAMPLES_BASE "Cube/glTF/Cube.gltf",
    SAMPLES_BASE "GearboxAssy/glTF/GearboxAssy.gltf",
    SAMPLES_BASE "MetalRoughSpheres/glTF/MetalRoughSpheres.gltf",
    SAMPLES_BASE "Monster/glTF/Monster.gltf",
    SAMPLES_BASE "NormalTangentTest/glTF/NormalTangentTest.gltf",
    SAMPLES_BASE "ReciprocatingSaw/glTF/ReciprocatingSaw.gltf",
    SAMPLES_BASE "RiggedFigure/glTF/RiggedFigure.gltf",
    SAMPLES_BASE "RiggedSimple/glTF/RiggedSimple.gltf",
    SAMPLES_BASE "SimpleMeshes/glTF/SimpleMeshes.gltf",
    SAMPLES_BASE "Suzanne/glTF/Suzanne.gltf",
    SAMPLES_BASE "TextureSettingsTest/glTF/TextureSettingsTest.gltf",
    SAMPLES_BASE "Triangle/glTF/Triangle.gltf",
    SAMPLES_BASE "TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf",
    SAMPLES_BASE "AnimatedTriangle/glTF/AnimatedTriangle.gltf",
    SAMPLES_BASE "TwoSidedPlane/glTF/TwoSidedPlane.gltf",
    SAMPLES_BASE "2CylinderEngine/glTF/2CylinderEngine.gltf",
    SAMPLES_BASE "VC/glTF/VC.gltf",
};
static size_t g_pathIndex = 0;

static std::string g_text;

shared_ptr<Material> createBoxMaterial();

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
    float offset = static_cast<float>(yoffset) * _zoomMag * -0.5f;
    offset = std::min(5.0f, offset);
    offset = std::max(-5.0f, offset);
    _orbitCamera.zoomOut(offset);
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
    using std::max;
    // account for near plane
    float zoom = _zoomMag + 0.2f;
    _orbitCamera.setZoom(zoom);
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
        calcBoundingBox(_scene.get());
        _orbitCamera.attach(_scene.get());
        _scene->addNode(_compass.node());
    }
    g_text.assign(path);
}

void Gltf2Test::loadNextPath() {
    loadSceneFromFile(nextPath());
}

void Gltf2Test::loadPrevPath() {
    loadSceneFromFile(prevPath());
}

void Gltf2Test::calcBoundingBox(Scene* scene) {
    _box = BoundingBox();
    if (scene) {
        for (size_t i = 0; i < scene->childCount(); ++i) {
            const auto& childBox = scene->childAt(i)->boundingBox();
            if (!childBox.empty()) {
                if (_box.empty()) {
                    _box = childBox;
                }
                else {
                    _box.merge(childBox);
                }
            }
        }
    }
    auto v1 = glm::abs(_box.max);
    auto v2 = glm::abs(_box.min);
    auto v = vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
    _zoomMag = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

    auto prim = createWireframeBoxPrimitive(_box);
    auto material = createBoxMaterial();
    if (prim && material) {
        prim->setMaterial(material);
        auto node = Node::create("box");
        node->addComponent(MeshRenderer::create(Mesh::create(prim)));
        scene->addNode(node);
    }
}

shared_ptr<Material> createBoxMaterial() {
    static constexpr char* VERT_PATH = "res/shaders/lamp.vert";
    static constexpr char* FRAG_PATH = "res/shaders/lamp.frag";
    auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
    if (effect) {
        auto tech = Technique::create(effect);
        tech->setAttribute("a_position", AttributeSemantic::POSITION);
        tech->setSemanticUniform("mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
        auto f = [](Effect& effect, const Uniform* uniform) {effect.setValue(uniform, glm::vec3(1, 0, 0)); };
        tech->setUniform("color", MaterialParameter::create("color", f));

        auto& state = tech->renderState();
        state.setDepthTest(true);
        state.setCulling(true);

        return Material::create(tech);
    }
    return nullptr;
}
}
}
