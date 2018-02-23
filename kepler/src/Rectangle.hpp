#pragma once

#include "BaseMath.hpp"

namespace kepler {

/// 2D axis aligned rectangle.
struct Rectangle {
    glm::vec2 pos;  // x, y position
    glm::vec2 size; // width and height

    Rectangle() noexcept;
    Rectangle(float x, float y, float width, float height) noexcept;
    Rectangle(const glm::vec2& pos, const glm::vec2& size) noexcept;
    Rectangle(const Rectangle& copy) = default;

    // access

    inline float x() const noexcept;
    inline float y() const noexcept;
    inline float width() const noexcept;
    inline float height() const noexcept;

    inline float top() const noexcept;
    inline float bottom() const noexcept;
    inline float left() const noexcept;
    inline float right() const noexcept;

    // modifiers
    void move(glm::vec2 delta) noexcept;

    // tests

    bool contains(glm::vec2 point) const noexcept;

    Rectangle& operator= (const Rectangle& r) = default;

    bool operator== (const Rectangle& r) const noexcept {
        return pos == r.pos && size == r.size;
    }

    bool operator!= (const Rectangle& r) const noexcept {
        return !((*this) == r);
    }
};
}

#include "Rectangle.inl"
