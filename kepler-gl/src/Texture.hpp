#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {

class Texture {
public:

    enum class Type : GLenum {
        TEXTURE_2D = GL_TEXTURE_2D,
        TEXTURE_3D = GL_TEXTURE_3D,
        TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
        TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP
    };

    // Use Texture::create2D() to create a texture.
    Texture(TextureHandle handle, Type type, int width, int height);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    void bind(GLenum textureUnit) const noexcept;

    Type type() const;
    TextureHandle handle() const;

    int width() const;
    int height() const;

    shared_ptr<Sampler> sampler() const;

    void setSampler(const shared_ptr<Sampler>& sampler);

    virtual ~Texture() noexcept;

    /// Creates a 2D texture from an Image.
    /// @param[in] image           The image to create the texture from.
    /// @param[in] internalFormat  Internal format of the texture (see glTexImage2D docs). GL_RGB, GL_RGBA...
    /// @param[in] generateMipmaps True if mipmaps should be generated automatically; false otherwise.
    /// @return Shared ptr to the texture. May be null if there was an error.
    static shared_ptr<Texture> create2D(Image* image, int internalFormat, bool generateMipmaps = false);

    // TODO immutable 2D texture

private:
    TextureHandle _handle;
    Type _type;
    int _width;
    int _height;
    shared_ptr<Sampler> _sampler;
};

} // namespace gl
} // namespace kepler
