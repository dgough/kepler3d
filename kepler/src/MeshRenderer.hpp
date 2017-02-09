#pragma once

#include "Base.hpp"
#include "DrawableComponent.hpp"

namespace kepler {

    // DrawableComponent for rendering a Mesh.
    class MeshRenderer : public virtual DrawableComponent {
        ALLOW_MAKE_SHARED(MeshRenderer);
    public:
        virtual ~MeshRenderer() noexcept;

        static ref<MeshRenderer> create(ref<Mesh> mesh);

        void draw();

        void onNodeChanged(const ref<Node>& oldNode, const ref<Node>& newNode) override;

        ref<Mesh> mesh() const;

        const std::string& typeName() const override;

    public:
        MeshRenderer(const MeshRenderer&) = delete;
        MeshRenderer& operator=(const MeshRenderer&) = delete;
    private:
        explicit MeshRenderer(ref<Mesh> mesh);
    private:
        // for now, only have 1 mesh per renderer
        ref<Mesh> _mesh;
    };
}
