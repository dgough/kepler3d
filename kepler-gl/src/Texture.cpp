#include "stdafx.h"
#include "Texture.hpp"
#include "Image.hpp"
#include "Sampler.hpp"

namespace kepler {
namespace gl {

static TextureHandle __currentTextureId = 0;

Texture::Texture(Type type)
    : _handle(0), _type(type), _width(0), _height(0) {
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
    if (_handle != 0) {
        glDeleteTextures(1, &_handle);
    }
}

shared_ptr<Texture> Texture::create2D(Image* image, int internalFormat, bool generateMipmaps) {
    if (image == nullptr) {
        return nullptr;
    }
    auto texture = std::make_shared<Texture>(Type::TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture->_handle);
    glBindTexture(GL_TEXTURE_2D, texture->_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image->width(), image->height(), 0, (GLenum)image->format(), image->type(), image->data());
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    texture->_width = image->width();
    texture->_height = image->height();
    return texture;
}
}
}
