#include "stdafx.h"
#include <OpenGL.hpp>
#include <iostream>

namespace kepler {
namespace gl {

#ifdef GLAD_DEBUG
static void check_gl_error(const char* name, void* funcptr, int len_args, ...) {
    GLenum error_code = glad_glGetError();
    if (error_code != GL_NO_ERROR) {
        fprintf(stderr, "GL_ERROR:: %d in %s\n", error_code, name);
    }
}
#endif

void replace_glad_callbacks() {
#ifdef GLAD_DEBUG
    glad_set_pre_callback(check_gl_error);
    glad_set_post_callback(check_gl_error);
#endif
}

} // namespace gl
} // namespace kepler
