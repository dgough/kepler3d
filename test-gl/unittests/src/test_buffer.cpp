#include "common_test.hpp"

#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>

#include <array>

using namespace kepler;
using namespace kepler::gl;

TEST(buffer, defaultConstructor) {
    VertexBuffer vertexBuffer;
    EXPECT_FALSE(static_cast<bool>(vertexBuffer));

    IndexBuffer indexBuffer;
    EXPECT_FALSE(static_cast<bool>(indexBuffer));
}

TEST(buffer, createVertexBuffer) {
    constexpr std::array<GLubyte, 6> vertices = {0, 0, 0, 0, 1, 0};
    VertexBuffer vertexBuffer(sizeof(vertices), vertices.data());
    EXPECT_TRUE(static_cast<bool>(vertexBuffer));
    VertexBuffer other(std::move(vertexBuffer));
    EXPECT_FALSE(static_cast<bool>(vertexBuffer));
    EXPECT_TRUE(static_cast<bool>(other));
    other.destroy();
    EXPECT_FALSE(static_cast<bool>(other));
}
