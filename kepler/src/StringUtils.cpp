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
        return stringEqual(s, prefix, ignoreCase);
    }

    bool endsWith(const std::string & subject, const char* suffix, bool ignoreCase) {
        if (suffix == nullptr || *suffix == '\0') {
            return true;
        }
        auto suffixLength = strlen(suffix);
        if (suffixLength > subject.length()) {
            return false;
        }
        const char* s = subject.c_str() + (subject.length() - suffixLength);
        return stringEqual(s, suffix, ignoreCase);
    }
}
