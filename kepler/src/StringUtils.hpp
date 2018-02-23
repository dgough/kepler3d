#pragma once

#include <string>

namespace kepler {

/// Returns true if subject string starts with prefix.
bool startsWith(const std::string& subject, const char* prefix, bool ignoreCase = false);
bool startsWith(const std::string& subject, const std::string& prefix, bool ignoreCase = false);

// Returns true if the subject ends with suffix.
bool endsWith(const std::string& subject, const char* suffix, bool ignoreCase = false);

// TODO equalsIgnoreCase

}