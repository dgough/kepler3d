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
#include <MeshPrimitive.hpp>
#include <MeshUtils.hpp>
#include <Scene.hpp>
#include <Technique.hpp>
#include <Material.hpp>

#include <iostream>
#include <algorithm>

using namespace kepler;
using glm::vec3;

static std::vector<const char*> g_paths{
    "../../glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
    "../../glTF-Sample-Models/2.0/Lantern/glTF/Lantern.gltf",
    "../../glTF-Sample-Models/2.0/Duck/glTF/Duck.gltf",
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
    "../../glTF-Sample-Models/2.0/Suzanne/glTF/Suzanne.gltf",
    "../../glTF-Sample-Models/2.0/TextureSettingsTest/glTF/TextureSettingsTest.gltf",
    "../../glTF-Sample-Models/2.0/Triangle/glTF/Triangle.gltf",
    "../../glTF-Sample-Models/2.0/TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf",
    "../../glTF-Sample-Models/2.0/AnimatedTriangle/glTF/AnimatedTriangle.gltf",
    "../../glTF-Sample-Models/2.0/TwoSidedPlane/glTF/TwoSidedPlane.gltf",
    "../../glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf",
    "../../glTF-Sample-Models/2.0/VC/glTF/VC.gltf",
};
static size_t g_pathIndex = 0;

static std::string g_text;

ref<Material> createBoxMaterial();

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

ref<Material> createBoxMaterial() {
    static constexpr char* VERT_PATH = "res/shaders/lamp.vert";
    static constexpr char* FRAG_PATH = "res/shaders/lamp.frag";
    auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
    if (effect) {
        auto tech = Technique::create(effect);
        tech->setAttribute("a_position", AttributeSemantic::POSITION);
        tech->setSemanticUniform("mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
        auto f = [](Effect& effect, const Uniform* uniform) {effect.setValue(uniform, glm::vec3(1, 0, 0));};
        tech->setUniform("color", MaterialParameter::create("color", f));

        auto& state = tech->renderState();
        state.setDepthTest(true);
        state.setCulling(true);

        return Material::create(tech);
    }
    return nullptr;
}
