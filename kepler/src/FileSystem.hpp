#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>


/// @defgroup filesystem FileSystem
namespace kepler {

    /// @ingroup filesystem
    /// Returns the directory name up to and including the trailing '/'.
    ///
    /// This is a lexical method so it does not verify that the directory exists.
    /// Back slashes will be converted to forward slashes.
    ///
    /// - `"res/image.png"` will return `"res/"`
    /// - `"image.png"` will return a blank string
    /// - `"c:\foo\bar\image.png"` will return `"c:/foo/bar/"`
    ///
    /// @param[in] path The file path. May be relative or absolute, forward or back slashes. May be nullptr.
    /// @return The directory name with the trailing '/'. Returns "" if path is nullptr or the path does not contain a directory.
    std::string getDirectoryName(const char* path);

    std::string joinPath(const std::string& path1, const std::string& path2);

    bool readTextFile(const char* path, std::string& destination);
    bool writeTextFile(const char* path, std::string& destination);

    /// Reads a binary file and loads the data into the given vector.
    /// @param[in] path The file path.
    /// @param[out] data The vector to load the data into. It will be resized.
    /// @return True if the file was read successfully; otherwise.
    template<class T>
    bool readBinaryFile(const char* path, std::vector<T>& data) {
        std::ifstream is(path, std::ios::binary);

        if (!is) {
            std::clog << "ERROR::READ_FILE " << path << std::endl;
            return false;
        }

        // find file size
        is.seekg(0, std::ios_base::end);
        std::size_t size = is.tellg();
        is.seekg(0, std::ios_base::beg);

        data.resize(size / sizeof(T));
        is.read(reinterpret_cast<char*>(&data[0]), size);
        is.close();
        return true;
    }

    template<class T>
    bool writeBinaryFile(const char* path, std::vector<T>& data) {
        std::ofstream os(path, std::ios::binary);
        if (!os) {
            std::clog << "ERROR::WRITE_FILE " << path << std::endl;
            return false;
        }
        os.write(reinterpret_cast<const char*>(&data[0]), sizeof(T) * data.size());
        os.close();
        return true;
    }
}
