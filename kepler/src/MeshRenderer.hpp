#pragma once

#include "Base.hpp"
#include "DrawableComponent.hpp"

namespace kepler {

    // DrawableComponent for rendering a Mesh.
    class MeshRenderer : public virtual DrawableComponent {
        ALLOW_MAKE_SHARED(MeshRenderer);
    public:
        virtual ~MeshRenderer() noexcept;

        static MeshRendererRef create(MeshRef mesh);

        void draw();

        void onNodeChanged(const NodeRef& oldNode, const NodeRef& newNode) override;

        MeshRef mesh() const;

        const std::string& typeName() const override;

    public:
        MeshRenderer(const MeshRenderer&) = delete;
        MeshRenderer& operator=(const MeshRenderer&) = delete;
    private:
        explicit MeshRenderer(MeshRef mesh);
    private:
        // for now, only have 1 mesh per renderer
        MeshRef _mesh;
    };
}
