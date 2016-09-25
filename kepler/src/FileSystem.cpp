#include "stdafx.h"
#include "FileSystem.hpp"

using std::string;

namespace kepler {
    string directoryName(const char* path) {
        // From gameplay3d
        if (path == nullptr || strlen(path) == 0) {
            return "";
        }
#ifdef _WIN32
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
        std::string dirname;
        size_t driveLength = strlen(drive);
        if (driveLength > 0) {
            dirname.reserve(driveLength + strlen(dir));
            dirname.append(drive);
            dirname.append(dir);
        }
        else {
            dirname.assign(dir);
        }
        std::replace(dirname.begin(), dirname.end(), '\\', '/');
        return dirname;
#else
        // dirname() modifies the input string so create a temp string
        std::string dirname;
        char* tempPath = new char[strlen(path) + 1];
        strcpy(tempPath, path);
        char* dir = ::dirname(tempPath);
        if (dir && strlen(dir) > 0) {
            dirname.assign(dir);
            // dirname() strips off the trailing '/' so add it back to be consistent with Windows
            dirname.append("/");
        }
        SAFE_DELETE_ARRAY(tempPath);
        return dirname;
#endif
    }

    string joinPath(const string& path1, const string& path2) {
        if (path1.length() == 0) {
            return path2;
        }
        // TODO

        char lastChar = path1[path1.length() - 1];
        if (lastChar == '/' || lastChar == '\\') {
            return path1 + path2;
        }
        return path1 + '/' + path2;
    }

    bool readTextFile(const char* path, std::string& destination) {
        try {
            std::ifstream file;
            file.exceptions(std::ifstream::badbit);
            file.open(path);
            if (!file) {
                std::clog << "ERROR::READ_TEXT_FILE " << path << std::endl;
                return false;
            }
            destination.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            file.close();
        }
        catch (const std::ifstream::failure& e) {
            std::clog << "ERROR::READ_TEXT_FILE " << path << " " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool writeTextFile(const char* path, std::string& destination) {
        std::ofstream os(path);
        if (!os) {
            std::clog << "ERROR::WRITE_TEXT_FILE " << path << std::endl;
            return false;
        }
        os.write(reinterpret_cast<const char*>(&destination[0]), destination.size());
        os.close();
        return true;
    }
}
