#pragma once

#include "Base.hpp"
#include "BaseMath.hpp"

#include <vector>
#include <functional>

namespace kepler {

    class MaterialBinding {
    public:
        MaterialBinding(NodeWeakRef node, MaterialRef material);
        virtual ~MaterialBinding() noexcept;

        void bind();

        void updateBindings(NodeRef node);
        void updateValues();

        const glm::mat4 getLocal() const;
        const glm::mat4 getModel() const;
        const glm::mat4 getView() const;
        const glm::mat4& getProjection() const;
        const glm::mat4 getModelView() const;
        const glm::mat4 getModelViewProjection() const;
        const glm::mat4 getModelInverse() const;
        const glm::mat4 getViewInverse() const;
        const glm::mat4 getProjectionInverse() const;
        const glm::mat4 getModelViewInverse() const;
        const glm::mat4 getModelViewProjectionInverse() const;
        const glm::mat3 getModelInverseTranspose() const;
        const glm::mat3 getModelViewInverseTranspose() const;
        const glm::mat4 getViewport() const;

    public:
        MaterialBinding(const MaterialBinding&) = delete;
        MaterialBinding& operator=(const MaterialBinding&) = delete;

    private:
        MaterialRef _material;
        NodeWeakRef _node;
        std::vector<std::function<void(void)>> _functions;
        std::vector<MaterialParameterRef> _values;
    };
}
