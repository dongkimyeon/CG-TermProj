#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform bool enableNightVision;
uniform float time;

void main()
{
    vec3 color = texture(screenTexture, TexCoord).rgb;
    
    if (enableNightVision) {
        // 1. 밝기 계산 (luminance)
        float brightness = dot(color, vec3(0.299, 0.587, 0.114));
        
        // 2. 밝기 증폭 (야간투시 특성)
        brightness = pow(brightness, 0.4) * 1.2;
        
        // 3. 녹색 톤으로 변환
        vec3 nightVisionColor = vec3(0.1, brightness, 0.2);
        
        // 4. 비네팅 효과 (가장자리 어둡게)
        vec2 center = TexCoord - 0.5;
        float vignette = 1.0 - dot(center, center) * 1.5;
        vignette = smoothstep(0.3, 0.8, vignette);
        
        // 5. 스캔라인 효과
        float scanline = sin(TexCoord.y * 800.0 + time * 10.0) * 0.04 + 1.0;
        
        // 6. 노이즈 효과
        float noise = fract(sin(dot(TexCoord + time * 0.1, vec2(12.9898, 78.233))) * 43758.5453);
        noise = noise * 0.1 + 0.95;
        
        // 7. 최종 색상 합성
        color = nightVisionColor * vignette * scanline * noise;
        
        // 8. 십자선 효과 (중앙)
        vec2 crosshair = abs(TexCoord - 0.5);
        if ((crosshair.x < 0.002 && crosshair.y < 0.02) || 
            (crosshair.y < 0.002 && crosshair.x < 0.02)) {
            color = vec3(1.0, 1.0, 1.0);
        }
        
        // 9. 테두리 효과
        if (TexCoord.x < 0.02 || TexCoord.x > 0.98 || 
            TexCoord.y < 0.02 || TexCoord.y > 0.98) {
            color *= 0.5;
        }
    }
    
    FragColor = vec4(color, 1.0);
}
