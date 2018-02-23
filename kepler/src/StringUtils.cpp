#include "stdafx.h"
#include "StringUtils.hpp"

using std::string;

namespace kepler {

static inline char lowercase(char ch) {
    return ch >= 'A' && ch <= 'Z' ? ch | 0x20 : ch;
}

static inline bool stringEqual(const char* s1, const char* s2, bool ignoreCase) {
    while (*s1 != '\0' && *s2 != '\0') {
        if (ignoreCase) {
            if (lowercase(*s1) != lowercase(*s2)) {
                return false;
            }
        }
        else if (*s1 != *s2) {
            return false;
        }
        ++s1;
        ++s2;
    }
    return *s1 == *s2;
}

static inline bool equalsIgnoreLength(const char* subject, const char* prefix, bool ignoreCase) {
    while (*subject != '\0' && *prefix != '\0') {
        if (ignoreCase) {
            if (lowercase(*subject) != lowercase(*prefix)) {
                return false;
            }
        }
        else if (*subject != *prefix) {
            return false;
        }
        ++subject;
        ++prefix;
    }
    return true;
}

bool startsWith(const std::string& subject, const char* prefix, bool ignoreCase) {
    if (prefix == nullptr || *prefix == '\0') {
        return true;
    }
    if (strlen(prefix) > subject.length()) {
        return false;
    }
    const char* s = subject.c_str();
    return equalsIgnoreLength(s, prefix, ignoreCase);
}

bool startsWith(const std::string& subject, const std::string& prefix, bool ignoreCase) {
    if (subject.length() < prefix.length()) {
        return false;
    }
    return startsWith(subject, prefix.c_str(), ignoreCase);
}

bool endsWith(const std::string& subject, const char* suffix, bool ignoreCase) {
    if (suffix == nullptr || *suffix == '\0') {
        return true;
    }
    auto suffixLength = strlen(suffix);
    if (suffixLength > subject.length()) {
        return false;
    }
    const char* s = subject.c_str() + (subject.length() - suffixLength);
    return equalsIgnoreLength(s, suffix, ignoreCase);
}
}
