#version 330 core
precision mediump float;

uniform sampler2D s_baseMap;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambient;
uniform float shininess;
uniform float specularStrength;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_texcoord0;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_fragPos);

    float lightDistance = length(lightDir);
    lightDir = lightDir / lightDistance;

    float attenuation = 1.0 / (
        constantAttenuation +
        linearAttenuation * lightDistance +
        quadraticAttenuation * lightDistance * lightDistance);

    vec3 viewDir = normalize(-v_fragPos);
    vec3 halfVec = normalize(lightDir + viewDir);

    float d = max(dot(normal, lightDir), 0.0);
    float s = max(dot(normal, halfVec), 0.0);

    if (d == 0.0)
        s = 0.0;
    else
        s = pow(s, shininess) * specularStrength;

    vec3 diffuse = d * lightColor * attenuation;
    vec3 specular = s * lightColor * attenuation;

    vec3 color = (ambient + diffuse + specular) * vec3(texture(s_baseMap, v_texcoord0));
    fragColor = vec4(color, 1.0);
}
