#pragma once

#include "Base.hpp"

namespace kepler {

    class Image {
        ALLOW_MAKE_SHARED(Image);
    public:
        enum class Format {
            L = 0x1909, ///< Luminance
            LA = 0x190A, ///< Luminance Alpha
            RGB = 0x1907,
            RGBA = 0x1908
        };

        virtual ~Image() noexcept;
        
        /// Creates an Image from a file.
        static ImageRef createFromFile(const char* path, bool flipY = false);

        // Creates an Image from memory.
        static ImageRef create(int width, int height, Format format, const unsigned char* data);

        /// Saves this image as a png file.
        static bool savePNG(const char* path, int width, int height, Format format, const unsigned char* data);

        /// Saves this image to the given past as a PNG file.
        bool savePNG(const char* path);

        /// Returns the width of this Image.
        int getWidth() const;

        /// Returns the height of this Image.
        int getHeight() const;

        Format getFormat() const;

        unsigned char* getData() const;

        unsigned int getDataType() const;

    public:
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
    private:
        Image(int width, int height, Format format, unsigned char* data);
    private:
        int _width;
        int _height;
        Format _format;
        unsigned char* _data;
    };
}
