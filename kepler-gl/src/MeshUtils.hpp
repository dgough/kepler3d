#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>
#include <BaseMath.hpp>

namespace kepler {
namespace gl {

/// Creates a cube MeshPrimitive with position and normal attributes.
shared_ptr<MeshPrimitive> createLitCubePrimitive();

/// Creates a quad with position, normal and texture attributes.
/// The quad is vertical and facing the positive Z axis.
shared_ptr<MeshPrimitive> createTexturedLitQuadPrimitive(vec2 maxTexCoords);

shared_ptr<MeshPrimitive> createWireframeBoxPrimitive(const BoundingBox& box);

} // namespace gl
} // namespace kepler
