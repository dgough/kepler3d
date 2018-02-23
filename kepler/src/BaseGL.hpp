#pragma once

#ifdef _WIN32
#define APIENTRY __stdcall
#endif

//#define GLAD_DEBUG

// GLAD
#include <glad/glad.h>

namespace kepler {

using BufferHandle = GLuint;
using VertexBufferHandle = GLuint;
using IndexBufferHandle = GLuint;
using ProgramHandle = GLuint;
using TextureHandle = GLuint;
using SamplerHandle = GLuint;

void replace_glad_callbacks();
}
