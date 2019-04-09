#include "stdafx.h"
#include "Sampler.hpp"

namespace kepler {
namespace gl {

static TextureHandle __currentSamplerId = 0;

Sampler::Sampler(SamplerHandle handle) : _handle(handle) {
}

Sampler::~Sampler() noexcept {
    if (_handle) {
        glDeleteSamplers(1, &_handle);
    }
}

shared_ptr<Sampler> Sampler::create() {
    SamplerHandle handle;
    glGenSamplers(1, &handle);
    if (handle) {
        return std::make_shared<Sampler>(handle);
    }
    return nullptr;
}

void Sampler::bind(GLenum textureUnit) const {
    if (_handle != __currentSamplerId) {
        glBindSampler(textureUnit, _handle);
        __currentSamplerId = _handle;
    }
}

void Sampler::setWrapMode(Sampler::Wrap wrapS, Sampler::Wrap wrapT, Sampler::Wrap wrapR) {
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, (GLint)wrapS);
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, (GLint)wrapT);
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, (GLint)wrapR);
}

void Sampler::setFilterMode(Sampler::MinFilter minFilter, Sampler::MagFilter magFilter) {
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
    glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);
}

} // namespace gl
} // namespace kepler
