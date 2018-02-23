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
    auto tech = material.technique();
    auto& effect = *(tech->effect());
    tech->bind();

    // Most MaterialBindings will have at least one binding that uses the camera so get it here
    // instead of having the Node search for it.
    ref<Camera> camera = nullptr;
    Camera* cameraPtr = nullptr;
    if (auto scene = node.scene()) {
        camera = scene->activeCamera();
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
    auto tech = material.technique();
    auto effect = tech->effect();
    for (const auto& semantic : tech->semantics()) {
        const ref<MaterialParameter>& materialParam = semantic.second;
        if (materialParam->uniform() == nullptr) {
            // assert?
            continue;
        }

        switch (materialParam->semantic()) {
        case MaterialParameter::Semantic::LOCAL:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.localTransform().matrix());
            });
            break;
        case MaterialParameter::Semantic::MODEL:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.worldMatrix());
            });
            break;
        case MaterialParameter::Semantic::VIEW:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.viewMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::PROJECTION:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.projectionMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELVIEW:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelViewMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELVIEWPROJECTION:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelViewProjectionMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELINVERSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelInverseMatrix());
            });
            break;
        case MaterialParameter::Semantic::VIEWINVERSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.viewInverseMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::PROJECTIONINVERSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.projectionInverseMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELVIEWINVERSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelViewInverseMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELVIEWPROJECTIONINVERSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelViewProjectionInverseMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::MODELINVERSETRANSPOSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelInverseTransposeMatrix());
            });
            break;
        case MaterialParameter::Semantic::MODELVIEWINVERSETRANSPOSE:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), node.modelViewInverseTransposeMatrix(camera));
            });
            break;
        case MaterialParameter::Semantic::VIEWPORT:
            _functions.emplace_back([materialParam](const Effect& effect, const Node& node, const Camera* camera) {
                effect.setValue(materialParam->uniform(), IDENTITY_MATRIX); // TODO
            });
            break;
        case MaterialParameter::Semantic::NONE:
        default:
            break;
        }
    }
}

void MaterialBinding::updateValues(const Material& material) {
    material.technique()->findValues(_values);
}
}
