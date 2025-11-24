#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    // 십자 모양 크로스헤어 (크기 축소)
    float thickness = 3.0;     // 0.05 -> 0.02로 더 얇게
    float length = 0.15;        // 0.8 -> 0.15로 훨씬 짧게
    
    // 수평선과 수직선
    bool isHorizontal = abs(TexCoord.y) < thickness && abs(TexCoord.x) < length;
    bool isVertical = abs(TexCoord.x) < thickness && abs(TexCoord.y) < length;
    
    // 중심 빈 공간
    bool isCenter = abs(TexCoord.x) < 0.04 && abs(TexCoord.y) < 0.04;
    
    if ((isHorizontal || isVertical) && !isCenter) {
        // 가장자리는 더 어둡게
        float alpha = 1.0 - smoothstep(length * 0.7, length, max(abs(TexCoord.x), abs(TexCoord.y)));
        FragColor = vec4(0.0, 1.0, 0.0, alpha * 0.9);  // 녹색으로 변경하고 더 선명하게
    } else {
        discard;
    }
}
