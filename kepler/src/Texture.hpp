#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    class Texture {
        friend class Sampler;
        ALLOW_MAKE_SHARED(Texture);
    public:

        enum class Type {
            TEXTURE_2D = GL_TEXTURE_2D,
            TEXTURE_3D = GL_TEXTURE_3D,
            TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
            TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP
        };

        void bind(GLenum textureUnit) const noexcept;

        Type getType() const;
        TextureHandle getHandle() const;

        int getWidth() const;
        int getHeight() const;

        SamplerRef getSampler() const;

        void setSampler(SamplerRef sampler);
        
        virtual ~Texture() noexcept;
        static TextureRef create2D(ImageRef image, int internalFormat, bool generateMipmaps = false);
        // TODO immutable 2D texture
    public:
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
    private:
        Texture(Type type);
    private:
        TextureHandle _handle;
        Type _type;
        int _width;
        int _height;
        SamplerRef _sampler;
    };
}
