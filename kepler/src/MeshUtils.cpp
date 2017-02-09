#include "stdafx.h"
#include "MeshUtils.hpp"
#include "MeshPrimitive.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexAttributeAccessor.hpp"
#include "IndexAccessor.hpp"

#include <array>

namespace kepler {

    /// Creates a cube mesh primitive
    ref<MeshPrimitive> createLitCubePrimitive() {
        static constexpr float p = 0.5f;
        static constexpr float n = -0.5f;
        static constexpr std::array<GLfloat, 144> vertices = {
            // back face
            n, n, n, 0.0f, 0.0f, -1.f, // bottom left
            n, p, n, 0.0f, 0.0f, -1.f, // top left
            p, n, n, 0.0f, 0.0f, -1.f, // bottom right
            p, p, n, 0.0f, 0.0f, -1.f, // top right

            // right face
            p, n, n, 1.0f, 0.0f, 0.f, // bottom back
            p, p, n, 1.0f, 0.0f, 0.f, // top back
            p, n, p, 1.0f, 0.0f, 0.f, // bottom front
            p, p, p, 1.0f, 0.0f, 0.f, // top front

            // front face
            p, n, p, 0.0f, 0.0f, 1.f, // bottom right
            p, p, p, 0.0f, 0.0f, 1.f, // top right
            n, n, p, 0.0f, 0.0f, 1.f, // bottom left
            n, p, p, 0.0f, 0.0f, 1.f, // top left

            // left face
            n, n, p, -1.0f, 0.0f, 0.f, // bottom front
            n, p, p, -1.0f, 0.0f, 0.f, // top front
            n, n, n, -1.0f, 0.0f, 0.f, // bottom back
            n, p, n, -1.0f, 0.0f, 0.f, // top back

            // top face
            n, p, n, 0.f, 1.f, 0.f, // back left
            n, p, p, 0.f, 1.f, 0.f, // front left
            p, p, n, 0.f, 1.f, 0.f, // back right
            p, p, p, 0.f, 1.f, 0.f, // front right

            // bottom face
            n, n, n, 0.f, -1.f, 0.f, // back left
            p, n, n, 0.f, -1.f, 0.f, // back right
            n, n, p, 0.f, -1.f, 0.f, // front left
            p, n, p, 0.f, -1.f, 0.f, // front right
        };
        auto vbo = VertexBuffer::create(sizeof(vertices), vertices.data());
        static constexpr GLsizei stride = 6 * sizeof(GLfloat);
        static constexpr GLsizei count = 24;
        auto prim = MeshPrimitive::create(MeshPrimitive::Mode::TRIANGLES);
        prim->setAttribute(AttributeSemantic::POSITION, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 0, count));
        prim->setAttribute(AttributeSemantic::NORMAL, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 3 * sizeof(GLfloat), count));
        static constexpr std::array<GLubyte, 36> indices = {
            0, 1, 2, 2, 1, 3, // back face
            4, 5, 6, 6, 5, 7, // right face
            8, 9, 10, 10, 9, 11, // front face
            12, 13, 14, 14, 13, 15, // left face
            16, 17, 18, 18, 17, 19, // top face
            20, 21, 22, 22, 21, 23 // bottom face
        };
        auto indexBuffer = IndexBuffer::create(sizeof(indices), indices.data());
        auto indexAccessor = IndexAccessor::create(indexBuffer, static_cast<GLsizei>(indices.size()), IndexAccessor::UNSIGNED_BYTE, 0);
        prim->setIndices(indexAccessor);
        return prim;
    }

    ref<MeshPrimitive> createTexturedLitQuadPrimitive(glm::vec2 maxTexCoords) {
        static constexpr float p = 0.5f;
        static constexpr float n = -0.5f;
        static constexpr float z = 0.0f;
        const float s = maxTexCoords.x;
        const float t = maxTexCoords.y;
        static const std::array<GLfloat, 32> vertices = {
            n, p, z, 0.f, 0.f, 1.f, s, t,
            n, n, z, 0.f, 0.f, 1.f, s, z,
            p, p, z, 0.f, 0.f, 1.f, z, t,
            p, n, z, 0.f, 0.f, 1.f, z, z
        };
        auto vbo = VertexBuffer::create(sizeof(vertices), vertices.data());
        static constexpr GLsizei stride = 8 * sizeof(GLfloat);
        static constexpr GLsizei count = 4;
        auto prim = MeshPrimitive::create(MeshPrimitive::Mode::TRIANGLE_STRIP);
        prim->setAttribute(AttributeSemantic::POSITION, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 0, count));
        prim->setAttribute(AttributeSemantic::NORMAL, VertexAttributeAccessor::create(vbo, 3, GL_FLOAT, false, stride, 3 * sizeof(GLfloat), count));
        prim->setAttribute(AttributeSemantic::TEXCOORD_0, VertexAttributeAccessor::create(vbo, 2, GL_FLOAT, false, stride, 6 * sizeof(GLfloat), count));
        return prim;
    }

}
