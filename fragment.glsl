#version 330 core

precision mediump float;

uniform sampler2D textureSampler;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;  // 추가

uniform vec3 lightDir;
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

in vec2 UV;
in vec3 v_lightTs;
in vec3 v_viewTS;
in vec4 vertexColor;
in vec4 FragPosLightSpace; 

out vec4 color;

// Shadow 계산 함수
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // [0,1] 범위로 변환
    projCoords = projCoords * 0.5 + 0.5;
    
    // 범위 체크
    if(projCoords.z > 1.0)
        return 0.0;
    
    // 현재 fragment의 깊이
    float currentDepth = projCoords.z;
    
    // bias를 사용하여 shadow acne 방지
    // lightDir은 이미 올바른 방향이므로 그대로 사용
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // PCF (Percentage-Closer Filtering) 적용
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    // 파티클 렌더링 (기존 코드)
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
    
    vec3 normal;
    if (useNormalMap == 1) {
        normal = normalize(2.0 * texture(normalMap, UV).xyz - 1.0);
    } 
    else {
        normal = vec3(0.0, 0.0, 1.0);
    }
    
    vec3 viewDir = normalize(v_viewTS);
    vec3 lightDirTS = normalize(v_lightTs);
    
    // Diffuse
    float diffuseFactor = max(dot(normal, lightDirTS), 0.0);
    vec3 diffuse = diffuseFactor * lightColor * matDiff;
    
    // Specular
    vec3 halfDir = normalize(lightDirTS + viewDir);
    float specularFactor = pow(max(dot(normal, halfDir), 0.0), shininess);
    vec3 specular = specularFactor * specularStrength * lightColor * matSpec;
    
    // Ambient
    vec3 ambient = ambientStrength * matDiff;
    
    // Shadow 계산 - v_lightTs를 사용 (이미 변환된 라이트 방향)
    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDirTS);
    
    // 최종 색상 = ambient + (1 - shadow) * (diffuse + specular)
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    
    color = vec4(lighting, texColor.a * alphaValue);
}