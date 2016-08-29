#include "stdafx.h"
#include "AxisCompass.hpp"

#include "Node.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "MeshRenderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexAttributeAccessor.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "Logging.hpp"

namespace kepler {

    static constexpr float SCALE = 0.05f;
    static constexpr float X_POS = 0.9f;
    static constexpr float Y_POS = 0.85f;

    static const char* VERT_PATH = "../kepler/res/shaders/axis.v.glsl";
    static const char* FRAG_PATH = "../kepler/res/shaders/axis.f.glsl";

    static MeshPrimitiveRef createPrimitive();
    static MeshRef createMesh();
    static NodeRef createNode();

    AxisCompass::AxisCompass() {
        _node = createNode();
    }

    AxisCompass::AxisCompass(SceneRef scene) : AxisCompass() {
        if (_node) {
            _node->setScene(scene);
        }
    }

    void AxisCompass::draw() {
        if (_node) {
            if (auto renderer = _node->getDrawable()) {
                renderer->draw();
            }
        }
    }

    NodeRef AxisCompass::getNode() const {
        return _node;
    }

    void AxisCompass::setScene(SceneRef scene) {
        if (_node) {
            _node->setScene(scene);
        }
    }

    MeshPrimitiveRef createPrimitive() {
        static constexpr GLfloat vertices[] = {
            0.f, 0.f, 0.f, 0.f, 1.f, 0.f, // y-axis
            0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f, 0.f, 0.f, // x-axis
            1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.0f, 0.0f, 1.f, // z-axis
            0.f, 0.f, 1.f, 0.f, 0.0f, 1.f,
        };
        auto vbo = VertexBuffer::create(sizeof(vertices), vertices);
        static constexpr GLsizei stride = 6 * sizeof(GLfloat);
        static constexpr GLsizei count = 6;
        auto prim = MeshPrimitive::create(MeshPrimitive::Mode::LINES);
        prim->setAttribute(Attribute::Semantic::POSITION, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 0, count));
        prim->setAttribute(Attribute::Semantic::COLOR, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 3 * sizeof(GLfloat), count));
        return prim;
    }

    MeshRef createMesh() {
        auto prim = createPrimitive();
        if (prim) {
            auto mesh = Mesh::create(prim);
            mesh->setName("axis mesh");
            auto effect = Effect::createFromFile(VERT_PATH, FRAG_PATH);
            if (effect) {
                auto tech = Technique::create();
                tech->setEffect(effect);
                tech->setAttribute("a_position", "position", Attribute::Semantic::POSITION, MaterialParameter::Type::FLOAT_VEC3);
                tech->setAttribute("a_color", "color", Attribute::Semantic::COLOR, MaterialParameter::Type::FLOAT_VEC3);
                tech->setSemanticUniform("model", "model", MaterialParameter::Semantic::MODEL, MaterialParameter::Type::FLOAT_MAT4);
                tech->setSemanticUniform("view", "view", MaterialParameter::Semantic::VIEW, MaterialParameter::Type::FLOAT_MAT4);
                tech->setSemanticUniform("proj", "proj", MaterialParameter::Semantic::PROJECTION, MaterialParameter::Type::FLOAT_MAT4);

                auto& state = tech->getRenderState();
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

    NodeRef createNode() {
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
