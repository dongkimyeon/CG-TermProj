#version 330 core

in vec2 UV;
in vec3 v_lightTs;  // 탄젠트 스페이스의 라이트 방향
in vec3 v_viewTS;   // 탄젠트 스페이스의 뷰 방향

uniform sampler2D textureSampler;  // 디퓨즈 텍스처 (colorMap)
uniform sampler2D normalMap;       // 노멀 맵
uniform bool useVertexColor;
uniform float alphaValue;
uniform bool useNormalMap;  // 노멀 맵 사용 여부

// 라이트 속성
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

out vec4 FragColor;

void main() {
    if (useVertexColor) {
        // 버텍스 컬러 모드 (축 그리기용)
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else {
        // 디퓨즈 텍스처 샘플링 (colorMap)
        vec4 texColor = texture(textureSampler, UV);
        vec3 matDiff = texColor.rgb;
        
        // 노멀 벡터 결정
        vec3 normal;
        if (useNormalMap) {
            // 노멀 맵에서 법선 벡터 샘플링 및 탄젠트 스페이스로 변환
            // 범위 전환: [0, 1] -> [-1, 1]
            vec3 normalTex = texture(normalMap, UV).xyz;
            normal = normalize(normalTex * 2.0 - 1.0);
        } else {
            // 노멀 맵이 없으면 기본 노멀 (탄젠트 스페이스의 Z축 방향)
            normal = vec3(0.0, 0.0, 1.0);
        }
        
        // 탄젠트 스페이스에서 정규화된 벡터들
        vec3 view = normalize(v_viewTS);
        vec3 light = normalize(v_lightTs);
        
        // Diffuse Term (확산광)
        float diffuseFactor = max(dot(normal, light), 0.0);
        vec3 diff = diffuseFactor * lightColor * matDiff;
        
        // Specular Term (반사광) - Blinn-Phong 반사 모델
        vec3 matSpec = vec3(specularStrength);
        vec3 halfDir = normalize(light + view);
        float specFactor = pow(max(dot(normal, halfDir), 0.0), shininess);
        vec3 spec = specFactor * lightColor * matSpec;
        
        // Ambient Term (환경광)
        vec3 srcAmbi = vec3(ambientStrength);
        vec3 ambi = srcAmbi * matDiff;
        
        // 최종 색상 계산
        FragColor = vec4(diff + spec + ambi, texColor.a * alphaValue);
    }
}