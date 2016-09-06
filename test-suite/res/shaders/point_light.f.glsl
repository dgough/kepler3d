#version 330 core
precision mediump float;

uniform sampler2D s_baseMap;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambient;
uniform float shininess;

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_texcoord0;

layout(location = 0) out vec4 fragColor;

const float specularStrength = 0.5;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_fragPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

    vec3 viewDir = normalize(-v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 color = (ambient + diffuse + specular) * vec3(texture(s_baseMap, v_texcoord0));
    fragColor = vec4(color, 1.0);
}
