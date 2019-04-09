#include "stdafx.h"
#include "FileSystem.hpp"

namespace kepler {

using std::string;

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

bool fileExists(const char* path) {
    struct stat s;
    return stat(path, &s) == 0;
}

bool fileExists(const std::string& path) {
    return fileExists(path.c_str());
}

void readTextFile(const char* path, std::string& destination) {
    std::ifstream file;
    try {
        file.exceptions(std::ios::badbit | std::ios::failbit);
        file.open(path);
        destination.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.close();
    }
    catch (const std::ios::failure&) {
        if (!file.eof()) {
            throw;
        }
    }
}

void writeTextFile(const char* path, const std::string& text) {
    std::ofstream out;
    out.exceptions(std::ios::badbit | std::ios::failbit);
    out.open(path);
    out << text;
    out.close();
}

} // namespace kepler
