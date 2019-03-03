#include "stdafx.h"
#include "Rectangle.hpp"

namespace kepler {

Rectangle::Rectangle() noexcept {}
Rectangle::Rectangle(float x, float y, float width, float height) noexcept : pos(x, y), size(width, height) {}
Rectangle::Rectangle(const vec2& pos, const vec2& size) noexcept : pos(pos), size(size) {}

void Rectangle::move(vec2 delta) noexcept {
    pos += delta;
}

bool Rectangle::contains(vec2 point) const noexcept {
    return point.x >= pos.x && point.x <= (pos.x + size.x) && point.y >= pos.y && point.y <= (pos.y + size.y);
}
}
