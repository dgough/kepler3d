#include "stdafx.h"
#include "Rectangle.hpp"

namespace kepler {

    Rectangle::Rectangle() noexcept {}
    Rectangle::Rectangle(float x, float y, float width, float height) noexcept : pos(x, y), size(width, height) {}
    Rectangle::Rectangle(const glm::vec2 & pos, const glm::vec2 & size) noexcept : pos(pos), size(size) {}

    float Rectangle::x() const noexcept
    {
        return pos.x;
    }

    float Rectangle::y() const noexcept
    {
        return pos.y;
    }

    float Rectangle::width() const noexcept {
        return size.x;
    }

    float Rectangle::height() const noexcept {
        return size.y;
    }
    bool Rectangle::contains(glm::vec2 point) const noexcept {
        return point.x >= pos.x && point.x <= (pos.x + size.x) && point.y >= pos.y && point.y <= (pos.y + size.y);
    }
}
