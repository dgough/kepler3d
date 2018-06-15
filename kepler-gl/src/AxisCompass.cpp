#include "stdafx.h"
#include "AxisCompass.hpp"

#include "Node.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "MeshRenderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexAttributeAccessor.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "Logging.hpp"

#include <array>

namespace kepler {
namespace gl {

static constexpr float SCALE = 0.05f;
static constexpr float X_POS = 0.9f;
static constexpr float Y_POS = 0.85f;

static const char* VERT_PATH = "../../kepler/res/shaders/axis.vert";
static const char* FRAG_PATH = "../../kepler/res/shaders/axis.frag";

static shared_ptr<MeshPrimitive> createPrimitive();
static shared_ptr<Mesh> createMesh();
static shared_ptr<Node> createNode();

AxisCompass::AxisCompass() {
    _node = createNode();
}

AxisCompass::AxisCompass(Scene* scene) : AxisCompass() {
    if (_node) {
        scene->addNode(_node);
    }
}

shared_ptr<Node> AxisCompass::node() const {
    return _node;
}

shared_ptr<MeshPrimitive> createPrimitive() {
    // x, y, z, r, g, b
    static constexpr std::array<GLubyte, 36> vertices = {
        0, 0, 0, 0, 1, 0, // y-axis
        0, 1, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0, // x-axis
        1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, // z-axis
        0, 0, 1, 0, 0, 1,
    };
    auto vbo = VertexBuffer::create(sizeof(vertices), vertices.data());
    static constexpr GLsizei stride = 6 * sizeof(GLubyte);
    static constexpr GLsizei count = 6;
    auto prim = MeshPrimitive::create(MeshPrimitive::Mode::LINES);
    prim->setAttribute(AttributeSemantic::POSITION, VertexAttributeAccessor::create(vbo, 3, GL_UNSIGNED_BYTE, false, stride, 0, count));
    prim->setAttribute(AttributeSemantic::COLOR, VertexAttributeAccessor::create(vbo, 3, GL_UNSIGNED_BYTE, false, stride, 3 * sizeof(GLubyte), count));
    return prim;
}

shared_ptr<Mesh> createMesh() {
    auto prim = createPrimitive();
    if (prim) {
        auto mesh = Mesh::create(prim);
        mesh->setName("axis mesh");
        auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
        if (effect) {
            auto tech = Technique::create();
            tech->setEffect(effect);
            tech->setAttribute("a_position", AttributeSemantic::POSITION);
            tech->setAttribute("a_color", AttributeSemantic::COLOR);
            tech->setSemanticUniform("model", MaterialParameter::Semantic::MODEL);
            tech->setSemanticUniform("view", MaterialParameter::Semantic::VIEW);
            tech->setSemanticUniform("proj", MaterialParameter::Semantic::PROJECTION);

            auto& state = tech->renderState();
            state.setDepthTest(true);
            //state.setLineWidth(4.f);

            auto material = Material::create(tech);
            prim->setMaterial(material);
            return mesh;
        }
        else {
            loge("Axis Compass::shader error");
        }
    }
    return nullptr;
}

shared_ptr<Node> createNode() {
    auto mesh = createMesh();
    if (mesh) {
        auto node = Node::create("compass");
        node->addComponent(MeshRenderer::create(mesh));
        node->translate(X_POS, Y_POS, 0.f);
        node->scale(SCALE);
        return node;
    }
    return nullptr;
}
}
}
