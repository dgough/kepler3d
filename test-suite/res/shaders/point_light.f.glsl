#version 330 core
precision mediump float;

uniform sampler2D s_baseMap;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambient;

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_texcoord0;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_fragPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
    vec3 color = (ambient + diffuse) * vec3(texture(s_baseMap, v_texcoord0));
    fragColor = vec4(color, 1.0);
}
