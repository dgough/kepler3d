#include "stdafx.h"
#include "Button.hpp"

namespace kepler {

Button::Button() {
}

Button::Button(const std::string& text) : _text(text) {
}

Button::Button(const std::string& text, const Rectangle& rect) : _text(text), _rect(rect) {
}

Button::~Button() noexcept {
}

ref<Button> Button::create() {
    return std::make_shared<Button>();
}

void Button::setOnClick(const std::function<void()>& callback) {
    _callback = callback;
}
void Button::callOnClick() const {
    if (_callback) {
        _callback();
    }
}
const std::string& Button::text() const {
    return _text;
}
void Button::setText(const char* text) {
    _text.assign(text);
}
void Button::setText(const std::string& text) {
    _text.assign(text);
}
const Rectangle& Button::rect() const {
    return _rect;
}
}
