#include "stdafx.h"
#include "DrawableComponent.hpp"

namespace kepler {

    DrawableComponent::DrawableComponent() {
    }

    DrawableComponent::~DrawableComponent() noexcept {
    }
    bool DrawableComponent::isDrawable() const {
        return true;
    }
}
