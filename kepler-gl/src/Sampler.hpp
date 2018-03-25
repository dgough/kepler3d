#pragma once

#include <BaseGL.hpp>
#include "Texture.hpp"

namespace kepler {
namespace gl {

class Sampler {
public:

    enum class Wrap {
        REPEAT = GL_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT
    };

    enum class MinFilter {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
    };

    enum class MagFilter {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR
    };

    static constexpr MinFilter DEFAULT_MIN_FILTER = MinFilter::NEAREST_MIPMAP_LINEAR;
    static constexpr MagFilter DEFAULT_MAG_FILTER = MagFilter::LINEAR;
    static constexpr Wrap DEFAULT_WRAP = Wrap::REPEAT;

    /// Use Sampler::create instead.
    Sampler();
    virtual ~Sampler() noexcept;

    /// Creates a Texture Sampler and returns a shared pointer to it.
    static ref<Sampler> create();

    void bind(GLenum textureUnit) const noexcept;

    void setWrapMode(Sampler::Wrap wrapS, Sampler::Wrap wrapT, Sampler::Wrap wrapR = DEFAULT_WRAP);
    void setFilterMode(Sampler::MinFilter minFilter, Sampler::MagFilter magFilter);

    // TODO getters
    // TODO LOD, swizzle etc.

public:
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
private:
    SamplerHandle _handle;
};
}
}
