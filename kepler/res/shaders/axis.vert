#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 v_color;

void main() {
    vec4 v = proj * view * vec4(a_position, 0.0);
    gl_Position = model * vec4(v.xyz, 1.0);
    v_color = a_color;
}
