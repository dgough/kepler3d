#include "stdafx.h"
#include "Logging.hpp"
#include <iostream>

using std::clog;
using std::endl;

namespace kepler {

// TODO improve logging.
// TODO colored text output?

void logi(const char* message) {
    clog << "INFO::" << message << endl;
}

void logw(const char* message) {
    clog << "WARN::" << message << endl;
}

void logw(const char* message, const char* message2) {
    clog << "WARN::" << message << message2 << endl;
}

void loge(const char* message) {
    clog << "ERROR::" << message << endl;
}

void loge(const char* message, const char* message2) {
    clog << "ERROR::" << message << message2 << endl;
}
}
