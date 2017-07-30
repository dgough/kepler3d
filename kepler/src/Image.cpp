#include "stdafx.h"
#include "Image.hpp"
#include "BaseGL.hpp"

#include <SOIL2.h>
#include "Logging.hpp"

namespace kepler {

    static Image::Format toImageFormat(int soilFormat);
    static int toSoilFormat(Image::Format format);
    static void flipImageY(unsigned char* data, int width, int height, int format);

    Image::Image(int width, int height, Format format, unsigned char* data)
        : _width(width), _height(height), _format(format), _data(data) {
    }

    Image::~Image() noexcept {
        if (_data != nullptr) {
            SOIL_free_image_data(_data);
        }
    }

    ref<Image> Image::createFromFile(const char* path, bool flipY) {
        int width, height, format;
        unsigned char* data = SOIL_load_image(path, &width, &height, &format, SOIL_LOAD_AUTO);
        if (data == nullptr) {
            loge("TEXTURE::LOAD_IMAGE ", path);
            return nullptr;
        }
        if (flipY) {
            flipImageY(data, width, height, format);
        }
        return std::make_shared<Image>(width, height, toImageFormat(format), data);
    }

    ref<Image> Image::createFromFileMemory(const unsigned char* buffer, int bufferLength, bool flipY) {
        int width, height, format;
        unsigned char* data = SOIL_load_image_from_memory(buffer, bufferLength, &width, &height, &format, SOIL_LOAD_AUTO);
        if (data == nullptr) {
            loge("TEXTURE::LOAD_IMAGE_FROM_MEMORY");
            return nullptr;
        }
        if (flipY) {
            flipImageY(data, width, height, format);
        }
        return std::make_shared<Image>(width, height, toImageFormat(format), data);
    }

    ref<Image> Image::createFromMemory(int width, int height, Format format, const unsigned char* data) {
        int soilFormat = toSoilFormat(format);
        size_t size = width * height * soilFormat;
        unsigned char* d = (unsigned char*)malloc(size);
        if (d == nullptr) {
            return nullptr;
        }
        std::memcpy(d, data, size);
        return std::make_shared<Image>(width, height, format, d);
    }

    bool Image::savePNG(const char* path, int width, int height, Format format, const unsigned char* data) {
        return SOIL_save_image(path, SOIL_SAVE_TYPE_PNG, width, height, toSoilFormat(format), data) != 0;
    }

    bool Image::savePNG(const char* path) {
        return savePNG(path, _width, _height, _format, _data);
    }

    int Image::width() const {
        return _width;
    }

    int Image::height() const {
        return _height;
    }

    Image::Format Image::format() const {
        return _format;
    }

    unsigned char* Image::data() const {
        return _data;
    }

    GLenum Image::type() const {
        return GL_UNSIGNED_BYTE;
    }

    Image::Format toImageFormat(int soilFormat) {
        switch (soilFormat) {
        case SOIL_LOAD_L: return Image::Format::L;
        case SOIL_LOAD_LA: return Image::Format::LA;
        case SOIL_LOAD_RGB: return Image::Format::RGB;
        case SOIL_LOAD_RGBA: return Image::Format::RGBA;
        default: return Image::Format::RGBA;
        }
    }

    int toSoilFormat(Image::Format format) {
        switch (format) {
        case Image::Format::L: return SOIL_LOAD_L;
        case Image::Format::LA: return SOIL_LOAD_LA;
        case Image::Format::RGB: return SOIL_LOAD_RGB;
        case Image::Format::RGBA: return SOIL_LOAD_RGBA;
        default: return SOIL_LOAD_RGBA;
        }
    }

    void flipImageY(unsigned char* data, int width, int height, int format) {
        for (int y = 0; y * 2 < height; ++y) {
            int i1 = y * width * format;
            int i2 = (height - 1 - y) * width * format;
            for (int x = width * format; x > 0; --x) {
                unsigned char temp = data[i1];
                data[i1] = data[i2];
                data[i2] = temp;
                ++i1;
                ++i2;
            }
        }
    }
}
