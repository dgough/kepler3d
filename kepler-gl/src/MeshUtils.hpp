#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>
#include <glm/glm.hpp>

namespace kepler {
namespace gl {

/// Creates a cube MeshPrimitive with position and normal attributes.
ref<MeshPrimitive> createLitCubePrimitive();

/// Creates a quad with position, normal and texture attributes.
/// The quad is vertical and facing the positive Z axis.
ref<MeshPrimitive> createTexturedLitQuadPrimitive(glm::vec2 maxTexCoords = glm::vec2(1));

ref<MeshPrimitive> createWireframeBoxPrimitive(const BoundingBox& box);

}
}