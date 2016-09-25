#version 330 core
layout (location = 0) in vec3 a_position;
uniform mat4 mvp;
void main() {
    gl_Position = mvp * vec4(a_position, 1.0);
}
