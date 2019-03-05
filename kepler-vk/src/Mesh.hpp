#pragma once

#include <BaseVk.hpp>
#include <BoundingBox.hpp>

#include <vector>

namespace kepler {
namespace vulkan {

/// A Mesh is a collection of MeshPrimitives.
class Mesh {
public:
    friend MeshRenderer;
    /// Use Mesh::create() instead.
    Mesh();
    explicit Mesh(const shared_ptr<MeshPrimitive>& primitive);
    virtual ~Mesh() noexcept;

    /// Creates an empty Mesh.
    static shared_ptr<Mesh> create();

    /// Creates a Mesh and adds the given MeshPrimitive.
    static shared_ptr<Mesh> create(const shared_ptr<MeshPrimitive>& primitive);

    /// Adds a MeshPrimitive to this mesh.
    /// @param[in] primitive The primitive to add.
    void addMeshPrimitive(const shared_ptr<MeshPrimitive>& primitive);

    void setName(const char* name);
    void setName(const std::string& name);

    /// Returns the MeshPrimitive at the given index.
    /// @param[in] index The index of the MeshPrimitive.
    shared_ptr<MeshPrimitive> primitiveAt(size_t index) const;
    MeshPrimitive* primitivePtr(size_t index) const;

    /// Returns the number of MeshPrimitives held by this Mesh.
    size_t primitiveCount() const;

    const BoundingBox& Mesh::boundingBox() const;

public:
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

private:

    void setNode(const shared_ptr<Node>& node);

    std::vector<shared_ptr<MeshPrimitive>> _primitives;
    std::unique_ptr<std::string> _name;
    BoundingBox _box;
};
}
}
