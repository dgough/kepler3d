#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord0;

uniform mat4 mvp;
uniform mat4 model;

out vec3 v_fragPos;
out vec3 v_normal;
out vec2 v_texcoord0;

void main() {
    vec4 position = vec4(a_position, 1.0);
    gl_Position = mvp * position;
    v_fragPos = vec3(model * position);
    v_normal = a_normal;
    v_texcoord0 = a_texcoord0;
}
