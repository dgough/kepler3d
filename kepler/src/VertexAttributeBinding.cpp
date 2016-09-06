#include "stdafx.h"
#include "VertexAttributeBinding.hpp"
#include "MeshPrimitive.hpp"
#include "Technique.hpp"
#include "Effect.hpp"
#include "VertexAttributeAccessor.hpp"
#include "IndexAccessor.hpp"
#include "Logging.hpp"

namespace kepler {
    VertexAttributeBinding::VertexAttributeBinding() : _handle(0) {

    }
    VertexAttributeBinding::~VertexAttributeBinding() noexcept {
        if (_handle != 0) {
            glDeleteVertexArrays(1, &_handle);
        }
    }

    VertexAttributeBindingRef VertexAttributeBinding::create(const MeshPrimitiveRef& meshPrim, const TechniqueRef& technique) {
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

        auto effect = technique->getEffect();
        if (effect == nullptr) {
            glDeleteVertexArrays(1, &handle);
            return nullptr;
        }

        auto indices = meshPrim->getIndices();
        if (indices) {
            indices->bind();
        }

        for (const auto& attrib : technique->getAttirbutes()) {
            const std::string& shaderAttribName = attrib.first;

            auto location = effect->getAttribLocation(shaderAttribName);
            auto attribAccessor = meshPrim->getAttribute(attrib.second);
            if (location >= 0 && attribAccessor) {
                GLuint index = static_cast<GLuint>(location);
                attribAccessor->bind(index);
                glEnableVertexAttribArray(index);
            }
        }
        glBindVertexArray(0); // Unbind VAO

        auto binding = MAKE_SHARED(VertexAttributeBinding);
        binding->_handle = handle;
        return binding;
    }

    void VertexAttributeBinding::bind() {
        glBindVertexArray(_handle);
    }

    void VertexAttributeBinding::unbind() {
        glBindVertexArray(0);
    }
}
