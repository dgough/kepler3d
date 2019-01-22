#pragma once

#include <BaseVk.hpp>
#include <DrawableComponent.hpp>
#include <Bounded.hpp>

namespace kepler {
namespace vulkan {

class Mesh;

// DrawableComponent for rendering a Mesh.
class MeshRenderer : public virtual DrawableComponent, public Bounded {
public:
    /// Use MeshRenderer::create()
    explicit MeshRenderer(const shared_ptr<Mesh>& mesh);
    virtual ~MeshRenderer() noexcept;

    static shared_ptr<MeshRenderer> create(const shared_ptr<Mesh>& mesh);

    void draw();

    void onNodeChanged(const shared_ptr<Node>& oldNode, const shared_ptr<Node>& newNode) override;

    /// Returns the shared_ptr to the mesh.
    shared_ptr<Mesh> mesh() const;

    /// Gets the boundings box if found.
    bool getBoundingBox(BoundingBox& box) override;

    const std::string& typeName() const override;

public:
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;

private:
    // for now, only have 1 mesh per renderer
    shared_ptr<Mesh> _mesh;
};
}
}
