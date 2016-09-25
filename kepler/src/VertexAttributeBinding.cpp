#include "stdafx.h"
#include "VertexAttributeBinding.hpp"
#include "MeshPrimitive.hpp"
#include "Technique.hpp"
#include "Effect.hpp"
#include "VertexAttributeAccessor.hpp"
#include "IndexAccessor.hpp"
#include "Logging.hpp"

namespace kepler {
    VertexAttributeBinding::VertexAttributeBinding(GLuint handle) : _handle(handle) {
    }

    VertexAttributeBinding::~VertexAttributeBinding() noexcept {
        if (_handle != 0) {
            glDeleteVertexArrays(1, &_handle);
        }
    }

    std::unique_ptr<VertexAttributeBinding> VertexAttributeBinding::createUnique(const MeshPrimitiveRef& meshPrim, const TechniqueRef& technique) {
        if (meshPrim == nullptr || technique == nullptr) {
            return nullptr;
        }
        GLuint handle = 0;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glGenVertexArrays(1, &handle);
        if (handle == 0) {
            loge("glGenVertexArrays");
            return nullptr;
        }
        glBindVertexArray(handle);

        auto effect = technique->effect();
        if (effect == nullptr) {
            glDeleteVertexArrays(1, &handle);
            return nullptr;
        }
        auto indices = meshPrim->indices();
        if (indices) {
            indices->bind();
        }
        for (const auto& attrib : technique->attributes()) {
            const auto& shaderAttribName = attrib.first;
            auto location = effect->attribLocation(shaderAttribName);
            auto attribAccessor = meshPrim->attribute(attrib.second);
            if (location >= 0 && attribAccessor) {
                auto index = static_cast<GLuint>(location);
                attribAccessor->bind(index);
                glEnableVertexAttribArray(index);
            }
        }
        glBindVertexArray(0); // Unbind VAO

        return std::make_unique<VertexAttributeBinding>(handle);
    }

    void VertexAttributeBinding::bind() {
        glBindVertexArray(_handle);
    }

    void VertexAttributeBinding::unbind() {
        glBindVertexArray(0);
    }
}
