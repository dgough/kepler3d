#include "LightTest.hpp"
#include "MainMenu.hpp"

#include <OpenGL.hpp>
#include <App.hpp>
#include <GLTF2Loader.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <MeshRenderer.hpp>
#include <StringUtils.hpp>
#include <BmpFont.hpp>
#include <Material.hpp>
#include <Technique.hpp>
#include <Mesh.hpp>
#include <MeshPrimitive.hpp>
#include <MeshRenderer.hpp>
#include <MeshUtils.hpp>
#include <Image.hpp>
#include <Sampler.hpp>
#include <Logging.hpp>
#include <Performance.hpp>

#include <iostream>

namespace kepler {
namespace gl {

#define SAMPLES_BASE "../../../glTF-Sample-Models/2.0/"

static constexpr char* GLTF_PATH = SAMPLES_BASE "Duck/glTF/Duck.gltf";
static constexpr char* DUCK_TEXTURE_PATH = SAMPLES_BASE "Duck/glTF/DuckCM.png";
static constexpr char* FLOOR_TEXTURE_PATH = "res/textures/hardwood.jpg";

static std::string g_text;
static vec3 g_lightColor(1);

static shared_ptr<Node> createLamp();
static shared_ptr<Material> createCubeMaterial();
static shared_ptr<Material> createPointLightMaterial(const char* texture_path, shared_ptr<Node> lightNode);

LightTest::LightTest() = default;

void LightTest::start() {
    glClearColor(0.16f, 0.3f, 0.5f, 1.0f);
    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    app()->setSwapInterval(1);

    _font = BmpFont::createFromFile("res/fonts/arial-32.fnt");

    loadSceneFromFile(GLTF_PATH);

    auto lamp = createLamp();
    if (_scene && lamp) {
        auto material = createPointLightMaterial(DUCK_TEXTURE_PATH, lamp);
        if (!material) {
            return;
        }
        _scene->visit([material](Node* node) {
            auto renderer = node->component<MeshRenderer>();
            if (renderer) {
                auto prim = renderer->mesh()->primitiveAt(0);
                if (prim) {
                    prim->setMaterial(material);
                }
            }
        });

        _lightParent = _scene->createChild("lamp_parent");
        _lightParent->addNode(lamp);
        _scene->addNode(_lightParent);

        auto floorPrim = createTexturedLitQuadPrimitive(vec2(4.f));
        floorPrim->setMaterial(createPointLightMaterial(FLOOR_TEXTURE_PATH, lamp));
        auto floor = _scene->createChild("floor");
        floor->addComponent(MeshRenderer::create(Mesh::create(floorPrim)));
        floor->translateY(-1.f);
        floor->rotateX(-PI_OVER_2);
        floor->scale(20.f, 20.f, 1.f);
    }
    if (_scene) {
        _scene->addNode(_compass.node());
    }
}

void LightTest::update() {
    if (!_pause && _lightParent) {
        float rot = static_cast<float>(deltaTime() * glm::pi<double>());
        _lightParent->rotateY(rot);
    }
}

void LightTest::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene) {
        _scene->visit([](Node* node) {
            if (auto renderer = node->drawable()) {
                renderer->draw();
            }
        });
    }

    if (_font) {
        float y = 0.f;
        float lineHeight = static_cast<float>(_font->lineHeight());
        _font->drawText(g_text.c_str(), 0.f, y);
        _font->drawText("[r,g,b,w] - change light color", 0.f, y += lineHeight, g_lightColor);
        _font->drawText("[space] - pause", 0.f, y += lineHeight);
    }
}

void LightTest::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        MainMenu::gotoMainMenu();
    }
    if (action == PRESS) {
        switch (key) {
        case KEY_R:
            g_lightColor = vec3(1, 0, 0);
            break;
        case KEY_G:
            g_lightColor = vec3(0, 1, 0);
            break;
        case KEY_B:
            g_lightColor = vec3(0.f, 0.5f, 1.f);
            break;
        case KEY_W:
            g_lightColor = vec3(1);
            break;
        case KEY_SPACE:
            _pause = !_pause;
            break;
        case KEY_F:
            _orbitCamera.setZoom(10.f);
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
        }
    }
}

void LightTest::mouseEvent(double xpos, double ypos) {
    if (_moveCamera && app()->getMouseButton(LEFT_MOUSE)) {
        _orbitCamera.move(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void LightTest::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
        double x, y;
        app()->cursorPosition(&x, &y);
        _orbitCamera.start(static_cast<float>(x), static_cast<float>(y));
        _moveCamera = true;
    }
}

void LightTest::scrollEvent(double xoffset, double yoffset) {
    _orbitCamera.zoomOut(static_cast<float>(-yoffset));
}

void LightTest::loadSceneFromFile(const char* path) {
    if (path == nullptr || *path == '\0') {
        std::clog << "Invalid path" << std::endl;
        return;
    }

    _orbitCamera.detach();
    _scene.reset();

    GLTF2Loader loader;
    loader.setAutoLoadMaterials(false);
    loader.setCameraAspectRatio(app()->aspectRatio());
    _scene = loader.loadSceneFromFile(path);

    if (_scene) {
        _orbitCamera.attach(_scene.get());
    }
    g_text.assign(path);
}

static shared_ptr<Node> createLamp() {
    auto prim = createLitCubePrimitive();
    auto material = createCubeMaterial();
    if (prim && material) {
        prim->setMaterial(material);
        auto node = Node::create("lamp");
        node->scale(0.3f);
        node->translate(0.f, 1.f, -3.f);
        node->addComponent(MeshRenderer::create(Mesh::create(prim)));
        return node;
    }
    return nullptr;
}

static shared_ptr<Material> createCubeMaterial() {
    static constexpr char* VERT_PATH = "res/shaders/lamp.vert";
    static constexpr char* FRAG_PATH = "res/shaders/lamp.frag";
    auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
    if (effect) {
        auto tech = Technique::create(effect);
        tech->setAttribute("a_position", AttributeSemantic::POSITION);
        tech->setSemanticUniform("mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
        auto f = [](Effect& effect, const Uniform* uniform) {effect.setValue(uniform, g_lightColor); };
        tech->setUniform("color", MaterialParameter::create("color", f));

        auto& state = tech->renderState();
        state.setDepthTest(true);
        state.setCulling(true);

        return Material::create(tech);
    }
    return nullptr;
}

static shared_ptr<Material> createPointLightMaterial(const char* texture_path, shared_ptr<Node> lightNode) {
    static constexpr char* VERT = "res/shaders/point_light.vert";
    static constexpr char* FRAG = "res/shaders/point_light.frag";
    auto effect = Effect::createFromFile(VERT, FRAG);
    auto image = Image::createFromFile(texture_path);
    auto texture = Texture::create2D(image.get(), GL_RGB, true);
    if (!effect || !image || !texture) {
        loge("failed to load light material");
        return nullptr;
    }

    auto sampler = Sampler::create();
    sampler->setWrapMode(Sampler::Wrap::REPEAT, Sampler::Wrap::REPEAT);
    sampler->setFilterMode(Sampler::MinFilter::LINEAR_MIPMAP_LINEAR, Sampler::MagFilter::LINEAR);
    texture->setSampler(sampler);

    auto tech = Technique::create(effect);
    tech->setAttribute("a_position", AttributeSemantic::POSITION);
    tech->setAttribute("a_normal", AttributeSemantic::NORMAL);
    tech->setAttribute("a_texcoord0", AttributeSemantic::TEXCOORD_0);

    tech->setSemanticUniform("mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
    tech->setSemanticUniform("modelView", MaterialParameter::Semantic::MODELVIEW);
    tech->setSemanticUniform("normalMatrix", MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE);

    auto lightPos = MaterialParameter::create("lightPos",
        [lightNode](Effect& effect, const Uniform* uniform) {
        // light position in view space
        vec4 v = vec4(lightNode->worldTransform().translation(), 1.0f);
        v = lightNode->viewMatrix() * v;
        effect.setValue(uniform, vec3(v));
    });
    tech->setUniform("lightPos", lightPos);

    auto lightColor = MaterialParameter::create("lightColor");
    lightColor->setValue([](Effect& effect, const Uniform* uniform) {
        effect.setValue(uniform, g_lightColor);
    });
    tech->setUniform("lightColor", lightColor);
    tech->setUniform("shininess", MaterialParameter::create("shininess", 64.0f));
    tech->setUniform("specularStrength", MaterialParameter::create("specularStrength", 0.2f));
    tech->setUniform("ambient", MaterialParameter::create("ambient", vec3(0.2f)));
    tech->setUniform("s_baseMap", MaterialParameter::create("base_texture", texture));

    tech->setUniform("constantAttenuation", MaterialParameter::create("constantAttenuation", 1.f));
    tech->setUniform("linearAttenuation", MaterialParameter::create("linearAttenuation", 0.f));
    tech->setUniform("quadraticAttenuation", MaterialParameter::create("quadraticAttenuation", 0.0025f));

    auto& state = tech->renderState();
    state.setDepthTest(true);
    state.setCulling(true);
    return Material::create(tech);
}
}
}