#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec4 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;  // 추가
uniform vec3 eyePos;
uniform vec3 lightDir;

out vec2 UV;
out vec3 v_lightTs;
out vec3 v_viewTS;
out vec4 vertexColor;
out vec4 FragPosLightSpace;  // 추가

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    
    UV = aUV;
    vertexColor = aColor;
    
    // Shadow mapping용 좌표 계산
    FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
    
    if (aColor.a > 0.0) {
        gl_PointSize = 80.0;
    } else {
        gl_PointSize = 1.0;
    }

    vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    
    // lightDir은 태양에서 지면으로 향하는 방향이므로 -lightDir이 라이트에서 오는 방향
    vec3 worldLightDir = normalize(-lightDir);
    vec3 worldViewDir = normalize(eyePos - worldPos);
    
    v_lightTs = TBN * worldLightDir;
    v_viewTS = TBN * worldViewDir;
}