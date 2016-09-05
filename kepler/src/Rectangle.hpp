#pragma once

#include "BaseMath.hpp"

namespace kepler {

    struct Rectangle {
        glm::vec2 pos;  // x, y position
        glm::vec2 size; // width and height

        Rectangle() noexcept;
        Rectangle(float x, float y, float width, float height) noexcept;
        Rectangle(const glm::vec2& pos, const glm::vec2& size) noexcept;
        Rectangle(const Rectangle& copy) = default;

        float x() const noexcept;
        float y() const noexcept;
        float width() const noexcept;
        float height() const noexcept;

        bool contains(glm::vec2 point) const noexcept;

        Rectangle& operator= (const Rectangle& r) = default;

        bool operator== (const Rectangle& r) const {
            return pos == r.pos && size == r.size;
        }

        bool operator!= (const Rectangle& r) const {
            return !((*this) == r);
        }
    };
}
