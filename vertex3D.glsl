#version 330 core

// 입력 버텍스 속성
layout(location = 0) in vec3 aPos;      // 정점 위치
layout(location = 1) in vec2 aUV;       // UV 좌표
layout(location = 2) in vec3 aNormal;   // 노멀 벡터
layout(location = 3) in vec3 aTangent;  // 탄젠트 벡터

// 유니폼 변수
uniform mat4 model;     // 모델 행렬
uniform mat4 view;      // 뷰 행렬
uniform mat4 proj;      // 투영 행렬
uniform vec3 eyePos;    // 카메라 위치 (월드 공간)
uniform vec3 lightDir;  // 광원 방향 (월드 공간)

// 프래그먼트 셰이더로 전달할 변수
out vec2 UV;            // UV 좌표
out vec3 v_lightTs;     // 탄젠트 공간의 광원 방향
out vec3 v_viewTS;      // 탄젠트 공간의 뷰 방향

void main() {
    // 정점을 클립 공간으로 변환
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    
    // UV 좌표 전달
    UV = aUV;

    // 월드 공간에서의 정점 위치 계산
    vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
    
    // 월드 공간에서의 법선 행렬 계산 (스케일과 회전 고려)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    // 노멀과 탄젠트를 월드 공간으로 변환
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    
    // Gram-Schmidt 재직교화로 탄젠트 정제
    T = normalize(T - dot(T, N) * N);
    
    // 바이탄젠트 계산 (N과 T의 외적)
    vec3 B = cross(N, T);
    
    // TBN 행렬 생성 (월드 공간 -> 탄젠트 공간)
    // transpose를 사용하여 역행렬 계산 (직교 행렬이므로 transpose = inverse)
    mat3 TBN = transpose(mat3(T, B, N));
    
    // 광원 벡터와 뷰 벡터를 월드 공간에서 계산
    vec3 worldLightDir = normalize(-lightDir);  // 광원 방향 (정점에서 광원으로)
    vec3 worldViewDir = normalize(eyePos - worldPos);  // 뷰 방향 (정점에서 카메라로)
    
    // 탄젠트 공간으로 변환
    v_lightTs = TBN * worldLightDir;
    v_viewTS = TBN * worldViewDir;
}