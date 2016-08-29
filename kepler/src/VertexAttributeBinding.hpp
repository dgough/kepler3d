#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    /// Wrapper for VAO.
    class VertexAttributeBinding {
        ALLOW_MAKE_SHARED(VertexAttributeBinding);
    public:
        virtual ~VertexAttributeBinding() noexcept;

        static VertexAttributeBindingRef create(const MeshPrimitiveRef& meshPrim, const TechniqueRef& technique);

        void bind();
        void unbind();

    private:
        VertexAttributeBinding();
        VertexAttributeBinding(const VertexAttributeBinding&) = delete;
        VertexAttributeBinding& operator=(const VertexAttributeBinding&) = delete;
    private:

        GLuint _handle;
    };
}
