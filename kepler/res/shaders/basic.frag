#version 330 core
precision mediump float;

#ifdef HAS_BASE_COLOR_MAP
uniform sampler2D s_baseMap;
#endif

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambient;
uniform float shininess;
uniform float specularStrength;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform vec4 baseColorFactor;

vec2 u_MetallicRoughness;

in vec3 v_fragPos;

//#ifdef HAS_NORMALS
in vec3 v_normal;
//#endif

#ifdef HAS_UV
in vec2 v_texcoord0;
#endif

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

    vec3 color = (ambient + diffuse + specular) * vec3(baseColorFactor);

    #if defined(HAS_UV) && defined(HAS_BASE_COLOR_MAP)
        color *= vec3(texture(s_baseMap, v_texcoord0));
    #endif

    fragColor = vec4(color, 1.0);
}
