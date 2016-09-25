#pragma once

#include "Base.hpp"
#include "AttributeSemantic.hpp"
#include "MaterialBinding.hpp"

#include <map>

namespace kepler {

    /// A MeshPrimitive is the data required to draw a primitive with glDrawArrays or glDrawElements.
    class MeshPrimitive : public std::enable_shared_from_this<MeshPrimitive> {
        ALLOW_MAKE_SHARED(MeshPrimitive);
        friend Mesh;
    public:

        enum Mode {
            POINTS = 0,
            LINES = 1,
            LINE_LOOP = 2,
            LINE_STRIP = 3,
            TRIANGLES = 4,
            TRIANGLE_STRIP = 5,
            TRIANGLE_FAN = 6
        };

        virtual ~MeshPrimitive() noexcept;

        static MeshPrimitiveRef create(Mode mode);

        /// Returns the attribute with the given semantic. May be null.
        VertexAttributeAccessorRef attribute(AttributeSemantic semantic) const;

        /// Returns the indices for this mesh primitive. May be null.
        IndexAccessorRef indices() const;

        void setAttribute(AttributeSemantic semantic, VertexAttributeAccessorRef accessor);
        /// Sets the IndexAccessor.
        void setIndices(IndexAccessorRef indices);

        /// Returns the material this primitive is bound to. May return null.
        MaterialRef material() const;

        /// Sets the Material that will be used to draw with.
        void setMaterial(MaterialRef material);

        void draw();

    public:
        MeshPrimitive(const MeshPrimitive&) = delete;
        MeshPrimitive& operator=(const MeshPrimitive&) = delete;
    private:
        MeshPrimitive(Mode mode);

        void updateBindings();
        void setNode(NodeRef node);

    private:
        // The type of primitives to render. Allowed values are 0 (POINTS), 1 (LINES), 2 (LINE_LOOP), 3 (LINE_STRIP), 4 (TRIANGLES), 5 (TRIANGLE_STRIP), and 6 (TRIANGLE_FAN).
        Mode _mode;
        IndexAccessorRef _indices;
        std::map<AttributeSemantic, VertexAttributeAccessorRef> _attributes;
        MaterialRef _material;
        std::unique_ptr<MaterialBinding> _materialBinding;

        std::unique_ptr<VertexAttributeBinding> _vertexBinding;
        NodeWeakRef _node;
    };
}
