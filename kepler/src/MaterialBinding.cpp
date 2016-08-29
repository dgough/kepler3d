#include "stdafx.h"
#include "MaterialBinding.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "MeshPrimitive.hpp"
#include "Node.hpp"

#include <functional>

namespace kepler {

    MaterialBinding::MaterialBinding(NodeWeakRef node, MaterialRef material)
        : _material(material), _node(node) {
    }

    MaterialBinding::~MaterialBinding() noexcept {
    }

    void MaterialBinding::bind() {
        auto tech = _material->getTechnique();
        tech->bind();
        for (auto f : _functions) {
            f();
        }
        auto effect = tech->getEffect();
        for (auto& v : _values) {
            v->bind(effect);
        }
    }

    void MaterialBinding::updateBindings(NodeRef node) {
        updateValues();
        // TODO determine if the node is the same and do less work?
        _node = node;

        auto tech = _material->getTechnique();
        auto effect = tech->getEffect();

        for (auto& semantic : tech->getSemantics()) {
            const MaterialParameterRef& materialParam = semantic.second;

            if (materialParam->getUniform() == nullptr || materialParam->getUniform()->getEffect() != effect) {
                // materialParam->setUniform(effect->getUniform(uniformName));// TODO not needed?
            }

            if (materialParam->getUniform() == nullptr) {
                // assert?
                continue;
            }

            switch (materialParam->getSemantic()) {
            case MaterialParameter::Semantic::LOCAL:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getLocal());
                });
                break;
            case MaterialParameter::Semantic::MODEL:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModel());
                });
                break;
            case MaterialParameter::Semantic::VIEW:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getView());
                });
                break;
            case MaterialParameter::Semantic::PROJECTION:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getProjection());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEW:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelView());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWPROJECTION:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelViewProjection());
                });
                break;
            case MaterialParameter::Semantic::MODELINVERSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelInverse());
                });
                break;
            case MaterialParameter::Semantic::VIEWINVERSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getViewInverse());
                });
                break;
            case MaterialParameter::Semantic::PROJECTIONINVERSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getProjectionInverse());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWINVERSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelViewInverse());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWPROJECTIONINVERSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelViewProjectionInverse());
                });
                break;
            case MaterialParameter::Semantic::MODELINVERSETRANSPOSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelInverseTranspose());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getModelViewInverseTranspose());
                });
                break;
            case MaterialParameter::Semantic::VIEWPORT:
                _functions.push_back([this, effect, materialParam]() {
                    effect->setValue(materialParam->getUniform(), this->getViewport());
                });
                break;
            case MaterialParameter::Semantic::NONE:
            default:
                break;
            }
        }
    }

    void MaterialBinding::updateValues() {
        auto tech = _material->getTechnique();
        tech->findValues(_values);
    }

    const glm::mat4 MaterialBinding::getLocal() const {
        if (NodeRef node = _node.lock()) {
            return node->getLocalTransform().getMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModel() const {
        if (NodeRef node = _node.lock()) {
            return node->getWorldMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getView() const {
        if (NodeRef node = _node.lock()) {
            return node->getViewMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4& MaterialBinding::getProjection() const {
        if (NodeRef node = _node.lock()) {
            return node->getProjectionMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModelView() const {
        if (NodeRef node = _node.lock()) {
            return node->getModelViewMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModelViewProjection() const {
        if (NodeRef node = _node.lock()) {
            return node->getModelViewProjectionMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModelInverse() const {
        if (NodeRef node = _node.lock()) {
            return node->getModelInverseMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getViewInverse() const {
        if (NodeRef node = _node.lock()) {
            return node->getViewInverseMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getProjectionInverse() const {
        if (NodeRef node = _node.lock()) {
            return node->getProjectionInverseMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModelViewInverse() const {
        if (NodeRef node = _node.lock()) {
            return node->getModelViewInverseMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4 MaterialBinding::getModelViewProjectionInverse() const {
        if (NodeRef node = _node.lock()) {
            return node->getModelViewProjectionInverseMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat3 MaterialBinding::getModelInverseTranspose() const {
        if (NodeRef node = _node.lock()) {
            return glm::mat3(node->getModelInverseTransposeMatrix());
        }
        return glm::mat3();
    }

    const glm::mat3 MaterialBinding::getModelViewInverseTranspose() const {
        if (NodeRef node = _node.lock()) {
            return glm::mat3(node->getModelViewInverseTransposeMatrix());
        }
        return glm::mat3();
    }

    const glm::mat4 MaterialBinding::getViewport() const {
        // TODO
        if (NodeRef node = _node.lock()) {
            return glm::mat4();
        }
        return IDENTITY_MATRIX;
    }
}
