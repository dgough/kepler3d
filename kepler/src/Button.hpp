#pragma once

#include "Base.hpp"
#include <Rectangle.hpp>

#include <functional>
#include <string>

namespace kepler {

class Button {
public:
    Button();
    explicit Button(const std::string& text);
    Button(const std::string& text, const Rectangle& rect);

    virtual ~Button() noexcept;

    static shared_ptr<Button> create();


    void setOnClick(const std::function<void()>& callback);

    /// Calls the OnClick callback directly.
    void callOnClick() const;

    const std::string& text() const;
    void setText(const char* text);
    void setText(const std::string& text);

    const Rectangle& rect() const;


    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;
private:
    Rectangle _rect;
    std::string _text;
    std::function<void()> _callback;
};
}
