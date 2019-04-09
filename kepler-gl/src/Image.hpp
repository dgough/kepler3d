#pragma once

#include <BaseGL.hpp>

namespace kepler {
namespace gl {

/// Uncompressed image data.
class Image {
public:
    enum class Format {
        L = 0x1909, ///< Luminance
        LA = 0x190A, ///< Luminance Alpha
        RGB = 0x1907,
        RGBA = 0x1908
    };

    /// Use one of the static create methods instead.
    Image(int width, int height, Format format, unsigned char* data);
    virtual ~Image() noexcept;
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    /// Creates an Image from a file.
    static shared_ptr<Image> createFromFile(const char* path, bool flipY = false);

    /// Creates an Image from memory that was loaded from a file.
    /// The memory is the data loaded from a compressed image, like a PNG file.
    /// The main use of this method is to load embedded images that were base 64 encoded in a GLTF file.
    ///
    /// @param[in] buffer       Pointer to the memory.
    /// @param[in] bufferLength Length of the buffer.
    /// @param[in] flipY        True if the Y-axis should be flipped; false otherwise.
    /// @return shared_ptr to the image. Will be null if there was an error.
    static shared_ptr<Image> createFromFileMemory(const unsigned char* buffer, int bufferLength, bool flipY = false);

    // Creates an Image from uncompressed memory.
    static shared_ptr<Image> createFromMemory(int width, int height, Format format, const unsigned char* data);

    /// Saves this image as a png file.
    static bool savePNG(const char* path, int width, int height, Format format, const unsigned char* data);

    /// Saves this image to the given past as a PNG file.
    bool savePNG(const char* path);

    /// Returns the width of this Image.
    int width() const;

    /// Returns the height of this Image.
    int height() const;

    /// Returns the image format.
    Format format() const;

    /// Direct access to the underlying memory.
    /// @return Pointer to the image data.
    const unsigned char* data() const;

    /// Returns the data type. Currently always GL_UNSIGNED_BYTE.
    unsigned int type() const;

private:
    int _width;
    int _height;
    Format _format;
    unsigned char* _data;
};
}
}
