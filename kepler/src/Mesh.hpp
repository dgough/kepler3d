#pragma once

#include "Base.hpp"

#include <vector>

namespace kepler {

    /// A Mesh is a collection of MeshPrimitives.
    class Mesh {
        ALLOW_MAKE_SHARED(Mesh);
    public:
        virtual ~Mesh() noexcept;

        /// Creates an empty Mesh.
        static MeshRef create();

        /// Creates a Mesh and adds the given MeshPrimitive.
        static MeshRef create(MeshPrimitiveRef primitive);

        /// Adds a MeshPrimitive to this mesh.
        /// @param[in] primitive The primitive to add.
        void addMeshPrimitive(MeshPrimitiveRef primitive);

        void setName(const std::string& name);
        void setNode(NodeRef node); // TODO make this private?

        /// Returns the MeshPrimitive at the given index.
        /// @param[in] index The index of the MeshPrimitive.
        MeshPrimitiveRef getPrimitive(size_t index) const;
        MeshPrimitive* getPrimitivePtr(size_t index) const;

        /// Returns the number of MeshPrimitives held by this Mesh.
        size_t getPrimitiveCount() const;

    public:
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    private:
        Mesh();

    private:
        std::vector<MeshPrimitiveRef> _primitives;
        std::string _name;
    };
}
