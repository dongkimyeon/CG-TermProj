#version 330 core

precision mediump float;

// 텍스처 샘플러
uniform sampler2D textureSampler;  // 디퓨즈 텍스처
uniform sampler2D normalMap;       // 노멀 맵 텍스처

// 조명 관련 유니폼
uniform vec3 lightDir;              // 광원 방향 (월드 공간)
uniform vec3 lightColor;            // 광원 색상
uniform vec3 eyePos;                // 카메라 위치
uniform float ambientStrength;      // 환경광 강도
uniform float specularStrength;     // 반사광 강도
uniform float shininess;            // 반사광 샤이니니스
uniform int useNormalMap;           // 노멀 맵 사용 여부
uniform float alphaValue;           // 알파값

// Vertex 셰이더로부터 입력
in vec2 UV;                         // UV 좌표
in vec3 v_lightTs;                  // 탄젠트 공간의 광원 방향
in vec3 v_viewTS;                   // 탄젠트 공간의 뷰 방향

// 출력
out vec4 color;

void main()
{
	// 1. 디퓨즈 텍스처에서 색상 가져오기 (조명 계산에 사용될 표면 색상)
	vec4 texColor = texture(textureSampler, UV);
	vec3 matDiff = texColor.rgb;
	
	// 2. 스페큘러 계수
	vec3 matSpec = vec3(1.0, 1.0, 1.0);
	
	// 3. 노멀 벡터 계산 (조명 계산을 위한 표면 방향만 사용)
	vec3 normal;
	if (useNormalMap == 1) {
    normal = normalize(2.0 * texture(normalMap, UV).xyz - 1.0);
	} 
	else {
    normal = vec3(0.0, 0.0, 1.0);
	}
	
	// 4. 탄젠트 공간에서 정규화
	vec3 viewDir = normalize(v_viewTS);
	vec3 lightDirTS = normalize(v_lightTs);
	
	// 5. Diffuse Term (Lambert) - 디퓨즈 텍스처 색상 사용
	float diffuseFactor = max(dot(normal, lightDirTS), 0.0);
	vec3 diffuse = diffuseFactor * lightColor * matDiff;
	
	// 6. Specular Term (Blinn-Phong)
	vec3 halfDir = normalize(lightDirTS + viewDir);
	float specularFactor = pow(max(dot(normal, halfDir), 0.0), shininess);
	vec3 specular = specularFactor * specularStrength * lightColor * matSpec;
	
	// 7. Ambient Term - 디퓨즈 텍스처 색상 사용
	vec3 ambient = ambientStrength * matDiff;
	
	// 8. 최종 색상 = 조명 효과 (디퓨즈 + 스페큘러 + 앰비언트)
	// 주의: 노멀맵의 색상은 절대 여기에 포함되지 않음!
	color = vec4(diffuse + specular + ambient, texColor.a * alphaValue);
}