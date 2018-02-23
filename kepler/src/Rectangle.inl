#include "Rectangle.hpp"

namespace kepler {

inline float Rectangle::x() const noexcept {
    return pos.x;
}
inline float Rectangle::y() const noexcept {
    return pos.y;
}
inline float Rectangle::width() const noexcept {
    return size.x;
}
inline float Rectangle::height() const noexcept {
    return size.y;
}
inline float Rectangle::top() const noexcept {
    return pos.y;
}
inline float Rectangle::bottom() const noexcept {
    return pos.y + height();
}
inline float Rectangle::left() const noexcept {
    return pos.x;
}
inline float Rectangle::right() const noexcept {
    return pos.x + width();
}
}
