#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;



uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 eyePos, lightDir;


out vec2 UV;

out vec3 v_lightTs, v_viewTS;


void main() {
   
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    UV = aUV;

    // view vector 구하려고 worldPos 구하기
	vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
	// v_view = normalize(eyePos - worldPos);

	// 탄젠트 공간 노멀 매핑 위해 TBN 구하기
	vec3 Nor = normalize(transpose(inverse(mat3(model))) * aNormal);
	vec3 Tan = normalize(transpose(inverse(mat3(model))) * aTangent);
    
    // Gram-Schmidt 재직교화로 Tangent 정제
    Tan = normalize(Tan - dot(Tan, Nor) * Nor);
    
    // Bitangent 계산
    vec3 Bit = cross(Nor, Tan);
    
    // 탄젠트 공간 변환 행렬
	mat3 tbnMat = transpose(mat3(Tan, Bit, Nor));

	// 빛 벡터, 뷰 벡터를 탄젠트 공간으로 변환
	v_lightTs = tbnMat * normalize(-lightDir);  // 빛 방향을 음수로
	v_viewTS = tbnMat * normalize(eyePos - worldPos);
}