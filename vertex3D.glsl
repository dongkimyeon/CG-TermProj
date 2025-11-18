#version 330 core

// 입력 버텍스 속성
layout(location = 0) in vec3 aPos;      // 버텍스 위치
layout(location = 1) in vec2 aUV;       // UV 좌표
layout(location = 2) in vec3 aNormal;   // 노멀 벡터
layout(location = 3) in vec3 aTangent;  // 탄젠트 벡터
layout(location = 4) in vec4 aColor;    // 버텍스 색상 (파티클용)

// 유니폼 변수
uniform mat4 model;     // 모델 행렬
uniform mat4 view;      // 뷰 행렬
uniform mat4 proj;      // 투영 행렬
uniform vec3 eyePos;    // 카메라 위치 (월드 좌표)
uniform vec3 lightDir;  // 광원 방향 (월드 좌표)

// 프래그먼트 셰이더로 전달할 변수
out vec2 UV;         // UV 좌표
out vec3 v_lightTs;     // 탄젠트 공간의 광원 방향
out vec3 v_viewTS;      // 탄젠트 공간의 뷰 방향
out vec4 vertexColor;   // 버텍스 색상 (파티클용)

void main() {
    // 클립 좌표로 변환
    gl_Position = proj * view * model * vec4(aPos, 1.0);
  
    // UV 좌표 전달
    UV = aUV;
    
    // 버텍스 색상 전달 (파티클용 - 설정되지 않은 경우 기본값 0)
    vertexColor = aColor;
    
    // 점 크기 설정 (파티클용) - 파티클인 경우 더 큰 크기로 설정
    if (aColor.a > 0.0) {
        gl_PointSize = 50.0; // 파티클 크기를 50픽셀로 설정 (더 크게)
    } else {
        gl_PointSize = 1.0;  // 일반 점은 기본 크기
    }

    // 월드 좌표계에서 버텍스 위치 계산
    vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
    
    // 월드 좌표계에서 노멀 벡터 계산 (스케일링과 회전 고려)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    // 노말과 탄젠트를 월드 좌표로 변환
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    
    // Gram-Schmidt 정규화로 탄젠트 보정
    T = normalize(T - dot(T, N) * N);
    
    // 바이탄젠트 계산 (N과 T의 외적)
    vec3 B = cross(N, T);
 
    // TBN 행렬 구성 (월드 공간 -> 탄젠트 공간)
    // transpose를 사용하여 역행렬 계산 (직교 행렬이므로 transpose = inverse)
    mat3 TBN = transpose(mat3(T, B, N));
    
    // 광원 방향과 뷰 벡터를 월드 좌표계에서 계산
    vec3 worldLightDir = normalize(-lightDir);  // 광원 방향 (픽셀에서 광원으로)
    vec3 worldViewDir = normalize(eyePos - worldPos);  // 뷰 방향 (픽셀에서 카메라로)
    
    // 탄젠트 공간으로 변환
    v_lightTs = TBN * worldLightDir;
    v_viewTS = TBN * worldViewDir;
}