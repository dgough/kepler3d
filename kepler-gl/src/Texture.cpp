#include "stdafx.h"
#include "Texture.hpp"
#include "Image.hpp"
#include "Sampler.hpp"

namespace kepler {
namespace gl {

static TextureHandle __currentTextureId = 0;

Texture::Texture(TextureHandle handle, Type type, int width, int height)
    : _handle(handle), _type(type), _width(width), _height(height) {
}

void Texture::bind(GLenum textureUnit) const noexcept {
    //if (_handle != __currentTextureId) {
    glBindTexture(static_cast<GLenum>(_type), _handle);
    __currentTextureId = _handle;
    //}

    if (_sampler != nullptr) {
        _sampler->bind(textureUnit);
    }
}

Texture::Type Texture::type() const {
    return _type;
}

TextureHandle Texture::handle() const {
    return _handle;
}

int Texture::width() const {
    return _width;
}

int Texture::height() const {
    return _height;
}

shared_ptr<Sampler> Texture::sampler() const {
    return _sampler;
}

void Texture::setSampler(const shared_ptr<Sampler>& sampler) {
    _sampler = sampler;
}

Texture::~Texture() noexcept {
    if (_handle) {
        glDeleteTextures(1, &_handle);
    }
}

shared_ptr<Texture> Texture::create2D(Image* image, int internalFormat, bool generateMipmaps) {
    if (image == nullptr) {
        return nullptr;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    TextureHandle handle;
    glGenTextures(1, &handle);
    if (handle == 0) {
        return nullptr;
    }
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image->width(), image->height(), 0, (GLenum)image->format(), image->type(), image->data());
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return std::make_shared<Texture>(handle, Type::TEXTURE_2D, image->width(), image->height());
}

} // namespace gl
} // namespace kepler
