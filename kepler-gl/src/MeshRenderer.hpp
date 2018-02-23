#pragma once

#include <Base.hpp>
#include "DrawableComponent.hpp"
#include <Bounded.hpp>

namespace kepler {

class Mesh;

// DrawableComponent for rendering a Mesh.
class MeshRenderer : public virtual DrawableComponent, public Bounded {
public:
    /// Use MeshRenderer::create()
    explicit MeshRenderer(ref<Mesh> mesh);
    virtual ~MeshRenderer() noexcept;

    static ref<MeshRenderer> create(ref<Mesh> mesh);

    void draw();

    void onNodeChanged(const ref<Node>& oldNode, const ref<Node>& newNode) override;

    /// Returns the shared_ptr to the mesh.
    ref<Mesh> mesh() const;

    /// Gets the boundings box if found.
    bool getBoundingBox(BoundingBox& box) override;

    const std::string& typeName() const override;

public:
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;

private:
    // for now, only have 1 mesh per renderer
    ref<Mesh> _mesh;
};
}
