#pragma once

#include <BaseGL.hpp>
#include <BaseMath.hpp>
#include <string>

namespace kepler {
namespace gl {


/// BmpFont represents a bitmap font that can draw byte strings.
///
/// BmpFont supports loading Angel Code BMFonts. (Typically with the ".fnt" file extension)
class BmpFont {
public:
    /// Use BmpFont::createFromFile() instead.
    BmpFont();
    virtual ~BmpFont() noexcept;
    BmpFont(const BmpFont&) = delete;
    BmpFont& operator=(const BmpFont&) = delete;

    /// Creates a BmpFont from an Angel Code font file.
    static shared_ptr<BmpFont> createFromFile(const char* path);

    void drawText(const char* text, float x, float y, const vec3& color = vec3(1, 1, 1));

    int size() const;
    float sizeAsFloat() const;
    int lineHeight() const;
    int base() const;

    const std::string& face() const;

    float scale() const;
    void setScale(float scale);

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
}
}
