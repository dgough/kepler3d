#include "stdafx.h"
#include "VertexAttributeBinding.hpp"
#include "MeshPrimitive.hpp"
#include "Technique.hpp"
#include "Effect.hpp"
#include "VertexAttributeAccessor.hpp"
#include "IndexAccessor.hpp"
#include "Logging.hpp"

namespace kepler {
namespace gl {

VertexAttributeBinding::VertexAttributeBinding(GLuint handle) : _handle(handle) {
}

VertexAttributeBinding::VertexAttributeBinding(const MeshPrimitive& meshPrim, const Technique& technique, const Effect& effect) {
    glGenVertexArrays(1, &_handle);
    if (_handle == 0) {
        loge("glGenVertexArrays");
        return;
    }

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindVertexArray(_handle);

    meshPrim.bindIndices();
    for (const auto& attrib : technique.attributes()) {
        const auto& shaderAttribName = attrib.first;
        auto location = effect.attribLocation(shaderAttribName);
        auto attribAccessor = meshPrim.attribute(attrib.second);
        if (location >= 0 && attribAccessor) {
            auto index = static_cast<GLuint>(location);
            attribAccessor->bind(index);
            glEnableVertexAttribArray(index);
        }
    }
    glBindVertexArray(0); // Unbind VAO
}

VertexAttributeBinding::~VertexAttributeBinding() noexcept {
    if (_handle) {
        glDeleteVertexArrays(1, &_handle);
    }
}

VertexAttributeBinding::VertexAttributeBinding(VertexAttributeBinding&& other) noexcept : _handle(other._handle) {
    other._handle = 0;
}

VertexAttributeBinding& VertexAttributeBinding::operator=(VertexAttributeBinding&& other) noexcept {
    if (this != &other) {
        std::swap(_handle, other._handle);
        other.destroy();
    }
    return *this;
}

void VertexAttributeBinding::destroy() {
    if (_handle) {
        glDeleteVertexArrays(1, &_handle);
        _handle = 0;
    }
}

} // namespace gl
} // namespace kepler
