#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec4 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 lightDir;          // 방향광원의 방향 추가

out vec2 UV;
out vec3 v_worldPos;
out vec3 v_worldNormal;
out vec3 v_lightDirTS;          // Tangent space light direction
out vec3 v_viewTS;
out vec4 vertexColor;
out vec4 FragPosLightSpace;

void main() {
    vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
    v_worldPos = worldPos;
    
    gl_Position = proj * view * vec4(worldPos, 1.0);
    
    UV = aUV;
    vertexColor = aColor;
    
    // Light Space 좌표 계산
    FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    
    if (aColor.a > 0.0) {
        gl_PointSize = 80.0;
    } else {
        gl_PointSize = 1.0;
    }

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    // 월드 노말 출력
    v_worldNormal = N;
    
    mat3 TBN = transpose(mat3(T, B, N));
    
    // 방향광원: 모든 지점에서 동일한 방향
    vec3 worldLightDirection = normalize(lightDir);
    vec3 worldViewDir = normalize(eyePos - worldPos);
    
    // Tangent Space로 변환 (빛의 방향)
    v_lightDirTS = TBN * worldLightDirection;
    v_viewTS = TBN * worldViewDir;
}