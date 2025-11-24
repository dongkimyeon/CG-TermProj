#version 330 core

precision mediump float;

// 텍스처 샘플러
uniform sampler2D textureSampler;  // 디퓨즈 텍스처
uniform sampler2D normalMap;       // 노멀 맵 텍스처

// 조명 관련 유니폼
uniform vec3 lightDir;        // 광원 방향 (월드 좌표)
uniform vec3 lightColor;            // 광원 색상
uniform vec3 eyePos; // 카메라 위치
uniform float ambientStrength;      // 환경광 강도
uniform float specularStrength; // 반사광 강도
uniform float shininess;   // 반사광 샤이니니스
uniform int useNormalMap;    // 노멀 맵 사용 여부
uniform float alphaValue;         // 알파값
uniform bool useTexture;          // 텍스처 사용 여부
uniform vec3 aColor;
uniform vec3 objectColor;  // 오브젝트 색상 

// Vertex 셰이더로부터 입력
in vec2 UV;    // UV 좌표
in vec3 v_lightTs;        // 탄젠트 공간의 광원 방향
in vec3 v_viewTS;   // 탄젠트 공간의 뷰 방향
in vec4 vertexColor;           // 버텍스 색상 (파티클용)

// 출력
out vec4 color;

void main()
{
    // 파티클인지 확인 (vertexColor가 설정되어 있는 경우)
    if (vertexColor.a > 0.0 && !useTexture) {
        // 파티클 렌더링 - 원형 모양으로 렌더링
        vec2 coord = gl_PointCoord - vec2(0.5);  // 점 중심을 원점으로
        float dist = length(coord);
        
        // 원형 모양 (부드러운 가장자리)
        float alpha = 1.0 - smoothstep(0.0, 0.5, dist);
        
        // 중심이 더 밝고 가장자리가 투명한 그라디언트
        float brightness = 1.0 - dist * 2.0;
        brightness = max(0.0, brightness);
        
        // 최종 색상 (밝은 회색/흰색)
        vec3 finalColor = vertexColor.rgb * brightness;
        color = vec4(finalColor, alpha * vertexColor.a);
        
        return;
    }
    
    // 일반 오브젝트 렌더링
    // 1. 디퓨즈 텍스처에서 베이스 컬러값 얻기 (감마 공간에 있는 표준 방법)
    vec4 texColor = texture(textureSampler, UV);
    vec3 matDiff;

    if (!useTexture) {
        // objectColor가 설정되어 있으면 사용 (크로스헤어용)
        matDiff = (objectColor != vec3(0.0)) ? objectColor : aColor;
    }
    else
    {
        matDiff = texColor.rgb;
    }
    
    // 2. 스페큘러 재질
    vec3 matSpec = vec3(1.0, 1.0, 1.0);
    
    // 3. 노멀 맵에서 노멀 (탄젠트 좌표에 있는 표준 방법만 사용)
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
    
    // 5. Diffuse Term (Lambert) - 디퓨즈 텍스처 값을 사용
    float diffuseFactor = max(dot(normal, lightDirTS), 0.0);
    vec3 diffuse = diffuseFactor * lightColor * matDiff;
    
    // 6. Specular Term (Blinn-Phong)
    vec3 halfDir = normalize(lightDirTS + viewDir);
    float specularFactor = pow(max(dot(normal, halfDir), 0.0), shininess);
    vec3 specular = specularFactor * specularStrength * lightColor * matSpec;
    
    // 7. Ambient Term - 디퓨즈 텍스처 값을 사용
    vec3 ambient = ambientStrength * matDiff;
  
    // 8. 최종 결과 = 조명 효과 (디퓨즈 + 스페큘러 + 엠비언트)
    // 주의: 감마값은 나중에 처리 예정에 있으므로 어디에 포함되지 않음!
    color = vec4(diffuse + specular + ambient, texColor.a * alphaValue);
}