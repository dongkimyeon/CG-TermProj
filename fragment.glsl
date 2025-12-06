#version 330 core

precision mediump float;

uniform sampler2D textureSampler;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightDir;          // 방향광원의 방향
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform int useNormalMap;
uniform float alphaValue;
uniform bool useTexture;
uniform vec3 aColor;
uniform vec3 objectColor;

// 스포트라이트 파라미터 (방향광원에서는 사용 안 함)
uniform float spotCutoff;
uniform float spotOuterCutoff;
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;

in vec2 UV;
in vec3 v_worldPos;
in vec3 v_worldNormal;
in vec3 v_lightDirTS;           // Tangent space light direction
in vec3 v_viewTS;
in vec4 vertexColor;
in vec4 FragPosLightSpace; 

out vec4 color;

// 방향광원용 Shadow 계산 함수
float ShadowCalculation(vec4 fragPosLightSpace, vec3 worldNormal, vec3 lightDirection)
{
    // Perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // [0,1] 범위로 변환
    projCoords = projCoords * 0.5 + 0.5;
    
    // 범위 체크
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;
    
    // 현재 fragment의 깊이
    float currentDepth = projCoords.z;
    
    // 방향 광원에 최적화된 bias - 표면 각도에 따라 조정
    float bias = max(0.005 * (1.0 - abs(dot(worldNormal, -lightDirection))), 0.001);
    
    // PCF (Percentage-Closer Filtering) - 부드러운 그림자
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int samples = 0;
    
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            vec2 sampleCoord = projCoords.xy + vec2(x, y) * texelSize;
            if(sampleCoord.x >= 0.0 && sampleCoord.x <= 1.0 && 
               sampleCoord.y >= 0.0 && sampleCoord.y <= 1.0) {
                float pcfDepth = texture(shadowMap, sampleCoord).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                samples++;
            }
        }
    }
    
    if(samples > 0)
        shadow /= float(samples);
    
    return shadow;
}

void main()
{
    // 파티클 렌더링
    if (vertexColor.a > 0.0 && !useTexture) {
        vec2 coord = gl_PointCoord - vec2(0.5);
        float dist = length(coord);
        float alpha = 1.0 - smoothstep(0.0, 0.5, dist);
        float brightness = 1.0 - dist * 2.0;
        brightness = max(0.0, brightness);
        vec3 finalColor = vertexColor.rgb * brightness;
        color = vec4(finalColor, alpha * vertexColor.a);
        return;
    }
    
    // 일반 객체 렌더링
    vec4 texColor = texture(textureSampler, UV);
    vec3 matDiff;

    if (!useTexture) {
        matDiff = (objectColor != vec3(0.0)) ? objectColor : aColor;
    }
    else {
        matDiff = texColor.rgb;
    }
    
    vec3 matSpec = vec3(1.0, 1.0, 1.0);
    
    // Tangent space normal
    vec3 normalTS;
    if (useNormalMap == 1) {
        normalTS = normalize(2.0 * texture(normalMap, UV).xyz - 1.0);
    } 
    else {
        normalTS = vec3(0.0, 0.0, 1.0);
    }
    
    // 월드 공간 계산
    vec3 worldNormal = normalize(v_worldNormal);
    vec3 lightDirection = normalize(lightDir);  // 방향광원의 방향
    
    // Tangent space 계산
    vec3 viewDir = normalize(v_viewTS);
    vec3 lightDirTS = normalize(v_lightDirTS);
    
    // 방향광원은 감쇠 없음 (거리에 무관하게 일정한 밝기)
    float attenuation = 1.0;
    
    // 방향광원은 스포트라이트 강도 계산 불필요
    float intensity = 1.0;
    
    // Diffuse (Tangent Space)
    float diffuseFactor = max(dot(normalTS, -lightDirTS), 0.0);
    vec3 diffuse = diffuseFactor * lightColor * matDiff;
    
    // Specular (Tangent Space)
    vec3 halfDir = normalize(-lightDirTS + viewDir);
    float specularFactor = pow(max(dot(normalTS, halfDir), 0.0), shininess);
    vec3 specular = specularFactor * specularStrength * lightColor * matSpec;
    
    // Ambient
    vec3 ambient = ambientStrength * matDiff;
    
    // Shadow 계산 
    float shadow = ShadowCalculation(FragPosLightSpace, worldNormal, lightDirection);
    
    // 최종 색상 
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    
    color = vec4(lighting, texColor.a * alphaValue);
}