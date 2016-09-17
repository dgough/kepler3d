#include "stdafx.h"
#include "MaterialBinding.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "MeshPrimitive.hpp"
#include "Node.hpp"
#include "Scene.hpp"
#include "Camera.hpp"

#include <functional>

namespace kepler {

    void MaterialBinding::bind(const Node& node, const Material& material) {
        auto tech = material.getTechnique();
        auto& effect = *(tech->getEffect());
        tech->bind();

        // Most MaterialBindings will have at least one binding that uses the camera so get it here
        // instead of having the Node search for it.
        CameraRef camera = nullptr;
        Camera* cameraPtr = nullptr;
        if (auto scene = node.getScene()) {
            camera = scene->getActiveCamera();
            if (camera) {
                cameraPtr = camera.get();
            }
        }

        for (const auto& f : _functions) {
            f(effect, node, cameraPtr);
        }
        for (const auto& v : _values) {
            v->bind(effect);
        }
    }

    void MaterialBinding::updateBindings(const Material& material) {
        updateValues(material);
        auto tech = material.getTechnique();
        auto effect = tech->getEffect();
        for (const auto& semantic : tech->getSemantics()) {
            const MaterialParameterRef& materialParam = semantic.second;
            if (materialParam->getUniform() == nullptr) {
                // assert?
                continue;
            }

            switch (materialParam->getSemantic()) {
            case MaterialParameter::Semantic::LOCAL:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getLocalTransform().getMatrix());
                });
                break;
            case MaterialParameter::Semantic::MODEL:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getWorldMatrix());
                });
                break;
            case MaterialParameter::Semantic::VIEW:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getViewMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::PROJECTION:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getProjectionMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELVIEW:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelViewMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWPROJECTION:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelViewProjectionMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELINVERSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelInverseMatrix());
                });
                break;
            case MaterialParameter::Semantic::VIEWINVERSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getViewInverseMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::PROJECTIONINVERSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getProjectionInverseMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWINVERSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelViewInverseMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWPROJECTIONINVERSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelViewProjectionInverseMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::MODELINVERSETRANSPOSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelInverseTransposeMatrix());
                });
                break;
            case MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), node.getModelViewInverseTransposeMatrix(camera));
                });
                break;
            case MaterialParameter::Semantic::VIEWPORT:
                _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                    effect.setValue(materialParam->getUniform(), IDENTITY_MATRIX); // TODO
                });
                break;
            case MaterialParameter::Semantic::NONE:
            default:
                break;
            }
        }
    }

    void MaterialBinding::updateValues(const Material& material) {
        auto tech = material.getTechnique();
        tech->findValues(_values);
    }
}
