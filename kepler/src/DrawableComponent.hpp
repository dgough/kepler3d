#pragma once

#include "Base.hpp"
#include "Component.hpp"

namespace kepler {

/// A Node Component that can draw itself.
class DrawableComponent : public virtual Component {
public:
    DrawableComponent();
    virtual ~DrawableComponent() noexcept;

    virtual void draw() = 0;

    bool isDrawable() const override;

    DrawableComponent(const DrawableComponent&) = delete;
    DrawableComponent& operator=(const DrawableComponent&) = delete;
};
}
