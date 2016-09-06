#include "LightTest.hpp"
#include "MainMenu.hpp"

#include <BaseGL.hpp>
#include <App.hpp>
#include <GLTFLoader.hpp>
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

#include <iostream>

using namespace kepler;

static constexpr char* GLTF_PATH =
"res/glTF/duck/Duck.gltf";
//"res/glTF/CesiumMilkTruck/CesiumMilkTruck.gltf";
static constexpr char* DUCK_TEXTURE_PATH = "res/glTF/duck/DuckCM.png";

static std::string g_text;
static glm::vec3 _lightColor(1);

static NodeRef createLamp();
static MaterialRef createCubeMaterial();
static MaterialRef createPointLightMaterial(const char* texture_path, NodeRef lightNode);

LightTest::LightTest() : _moveCamera(false) {
}

LightTest::~LightTest() {
}

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
            auto renderer = node->getComponent<MeshRenderer>();
            if (renderer) {
                auto prim = renderer->getMesh()->getPrimitive(0);
                if (prim) {
                    prim->setMaterial(material);
                }
            }
        });

        _lightParent = _scene->createChild("lamp_parent");
        _lightParent->addNode(lamp);
        _scene->addNode(_lightParent);
    }
}

void LightTest::update() {
    if (_lightParent) {
        float rot = static_cast<float>(getDeltaTime() * glm::pi<double>());
        _lightParent->rotateY(rot);
    }
}

void LightTest::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene) {
        _scene->visit([](Node* node) {
            if (auto renderer = node->getDrawable()) {
                renderer->draw();
            }
        });
    }

    if (_font) {
        _font->drawText(g_text.c_str(), 0.f, 0.f);
        _font->drawText("[r,g,b,w] - change light color", 0.f, static_cast<float>(_font->getLineHeight()));
    }
    _compass.draw();
}

void LightTest::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        MainMenu::gotoMainMenu();
    }
    if (action == PRESS) {
        switch (key) {
        case KEY_R:
            _lightColor = glm::vec3(1, 0, 0);
            break;
        case KEY_G:
            _lightColor = glm::vec3(0, 1, 0);
            break;
        case KEY_B:
            _lightColor = glm::vec3(0.f, 0.5f, 1.f);
            break;
        case KEY_W:
            _lightColor = glm::vec3(1);
            break;
        }
    }

    // TODO press f to focus
}

void LightTest::mouseEvent(double xpos, double ypos) {
    if (_moveCamera && app()->getMouseButton(LEFT_MOUSE)) {
        _orbitCamera.move(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void LightTest::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
        double x, y;
        app()->getCursorPos(&x, &y);
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

    GLTFLoader loader;
    loader.setAutoLoadMaterials(false);
    loader.setCameraAspectRatio(app()->getAspectRatio());
    _scene = loader.loadSceneFromFile(path);

    if (_scene) {
        _orbitCamera.attach(_scene);
        _compass.setScene(_scene);
    }
    g_text.assign(path);
}

static NodeRef createLamp() {
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

static MaterialRef createCubeMaterial() {
    static constexpr char* VERT_PATH = "res/shaders/lamp.v.glsl";
    static constexpr char* FRAG_PATH = "res/shaders/lamp.f.glsl";
    auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
    if (effect) {
        auto tech = Technique::create(effect);
        tech->setAttribute("a_position", AttributeSemantic::POSITION);
        tech->setSemanticUniform("mvp", "mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
        auto param = MaterialParameter::create("color");
        param->setFunction([](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, _lightColor);
        });
        tech->setUniform("color", param);

        auto& state = tech->getRenderState();
        state.setDepthTest(true);
        state.setCulling(true);

        return Material::create(tech);
    }
    return nullptr;
}

static MaterialRef createPointLightMaterial(const char* texture_path, NodeRef lightNode) {
    static constexpr char* VERT = "res/shaders/point_light.v.glsl";
    static constexpr char* FRAG = "res/shaders/point_light.f.glsl";
    auto effect = Effect::createFromFile(VERT, FRAG);
    if (!effect)
        return nullptr;
    auto image = Image::createFromFile(texture_path);
    if (!image) {
        return nullptr;
    }
    auto texture = Texture::create2D(image, GL_RGB, true);
    if (!texture) {
        return nullptr;
    }
    auto tech = Technique::create(effect);
    tech->setAttribute("a_position", AttributeSemantic::POSITION);
    tech->setAttribute("a_normal", AttributeSemantic::NORMAL);
    tech->setAttribute("a_texcoord0", AttributeSemantic::TEXCOORD_0);

    tech->setSemanticUniform("mvp", "mvp", MaterialParameter::Semantic::MODELVIEWPROJECTION);
    tech->setSemanticUniform("model", "model", MaterialParameter::Semantic::MODEL);

    auto lightPos = MaterialParameter::create("lightPos");
    lightPos->setFunction([lightNode](Effect& effect, const Uniform* uniform) {
        effect.setValue(uniform, lightNode->getWorldTransform().getTranslation());
    });
    tech->setUniform("lightPos", lightPos);

    auto lightColor = MaterialParameter::create("lightColor");
    lightColor->setFunction([](Effect& effect, const Uniform* uniform) {
        effect.setValue(uniform, _lightColor);
    });
    tech->setUniform("lightColor", lightColor);

    auto ambient = MaterialParameter::create("ambient");
    ambient->setValue(glm::vec3(0.2f, 0.2f, 0.2f));
    tech->setUniform("ambient", ambient);

    auto baseMap = MaterialParameter::create("base_texture");
    baseMap->setTexture(texture);
    tech->setUniform("s_baseMap", baseMap);

    auto& state = tech->getRenderState();
    state.setDepthTest(true);
    state.setCulling(true);
    return Material::create(tech);
}
