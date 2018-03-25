#pragma once

#include <BaseGL.hpp>
#include "BoundingBox.hpp"

#include <vector>

namespace kepler {
namespace gl {

/// A Mesh is a collection of MeshPrimitives.
class Mesh {
public:
    friend MeshRenderer;
    /// Use Mesh::create() instead.
    Mesh();
    explicit Mesh(const ref<MeshPrimitive>& primitive);
    virtual ~Mesh() noexcept;

    /// Creates an empty Mesh.
    static ref<Mesh> create();

    /// Creates a Mesh and adds the given MeshPrimitive.
    static ref<Mesh> create(const ref<MeshPrimitive>& primitive);

    /// Adds a MeshPrimitive to this mesh.
    /// @param[in] primitive The primitive to add.
    void addMeshPrimitive(const ref<MeshPrimitive>& primitive);

    void setName(const char* name);
    void setName(const std::string& name);

    /// Returns the MeshPrimitive at the given index.
    /// @param[in] index The index of the MeshPrimitive.
    ref<MeshPrimitive> primitiveAt(size_t index) const;
    MeshPrimitive* primitivePtr(size_t index) const;

    /// Returns the number of MeshPrimitives held by this Mesh.
    size_t primitiveCount() const;

    const BoundingBox& Mesh::boundingBox() const;

public:
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

private:

    void setNode(const ref<Node>& node);

    std::vector<ref<MeshPrimitive>> _primitives;
    std::unique_ptr<std::string> _name;
    BoundingBox _box;
};
}
}