#include "common_test.hpp"

#include <Shader.hpp>

using namespace kepler;
using namespace kepler::gl;

TEST(shader, defaultConstructor) {
    Shader shader;
    EXPECT_FALSE(static_cast<bool>(shader));

    EXPECT_NE(GL_VERTEX_SHADER, shader.getType());
    EXPECT_NE(GL_FRAGMENT_SHADER, shader.getType());
}

TEST(shader, moveVertex) {
    Shader vertexShader(GL_VERTEX_SHADER);
    EXPECT_TRUE(static_cast<bool>(vertexShader));
    EXPECT_EQ(GL_VERTEX_SHADER, vertexShader.getType());

    Shader other = std::move(vertexShader);
    EXPECT_FALSE(static_cast<bool>(vertexShader));
    EXPECT_TRUE(static_cast<bool>(other));

    other.destroy();
    EXPECT_FALSE(static_cast<bool>(other));
}

TEST(shader, fragment) {
    Shader fragShader(GL_FRAGMENT_SHADER);
    EXPECT_TRUE(static_cast<bool>(fragShader));
    EXPECT_EQ(GL_FRAGMENT_SHADER, fragShader.getType());

    Shader other = std::move(fragShader);
    EXPECT_FALSE(static_cast<bool>(fragShader));
    EXPECT_TRUE(static_cast<bool>(other));

    other.destroy();
    EXPECT_FALSE(static_cast<bool>(other));
}
