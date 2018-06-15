#pragma once

#include <BaseGL.hpp>
#include "AttributeSemantic.hpp"
#include "MaterialBinding.hpp"
#include "BoundingBox.hpp"

#include <map>

namespace kepler {
namespace gl {

/// A MeshPrimitive is the data required to draw a primitive with glDrawArrays or glDrawElements.
class MeshPrimitive : public std::enable_shared_from_this<MeshPrimitive> {
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
    /// Use MeshPrimitive::create() instead.
    explicit MeshPrimitive(Mode mode);
    virtual ~MeshPrimitive() noexcept = default;
    MeshPrimitive(const MeshPrimitive&) = delete;
    MeshPrimitive& operator=(const MeshPrimitive&) = delete;

    static shared_ptr<MeshPrimitive> create(Mode mode);

    /// Returns the attribute with the given semantic. May be null.
    shared_ptr<VertexAttributeAccessor> attribute(AttributeSemantic semantic) const;

    /// Returns true if this primitive contains this semantic.
    bool hasAttribute(AttributeSemantic semantic) const;

    /// Binds the indices for this mesh's primitive if they exist.
    void bindIndices();

    void setAttribute(AttributeSemantic semantic, const shared_ptr<VertexAttributeAccessor>& accessor);
    /// Sets the IndexAccessor.
    void setIndices(const shared_ptr<IndexAccessor>& indices);

    /// Returns the material this primitive is bound to. May return null.
    shared_ptr<Material> material() const;

    /// Sets the Material that will be used to draw with.
    void setMaterial(const shared_ptr<Material>& material);

    const BoundingBox& boundingBox() const;

    void setBoundingBox(const glm::vec3& min, const glm::vec3& max);

    void draw();

private:
    void updateBindings();
    void setNode(const shared_ptr<Node>& node);

private:
    // The type of primitives to render. Allowed values are 0 (POINTS), 1 (LINES), 2 (LINE_LOOP), 3 (LINE_STRIP), 4 (TRIANGLES), 5 (TRIANGLE_STRIP), and 6 (TRIANGLE_FAN).
    Mode _mode;
    shared_ptr<IndexAccessor> _indices;
    std::map<AttributeSemantic, shared_ptr<VertexAttributeAccessor>> _attributes;
    shared_ptr<Material> _material;
    std::unique_ptr<MaterialBinding> _materialBinding;
    std::unique_ptr<VertexAttributeBinding> _vertexBinding;
    std::weak_ptr<Node> _node;
    BoundingBox _box;
};
}
}
