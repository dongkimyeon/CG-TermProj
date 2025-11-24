#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;

uniform vec2 crosshairCenter; // 화면 공간의 크로스헤어 중심 위치

out vec4 FragColor;
out vec2 TexCoord;

void main()
{
    // aPos는 크로스헤어의 로컬 좌표 (-1 ~ 1 범위)
    vec2 finalPos = crosshairCenter + aPos * 0.02; // 크로스헤어 크기 조절
    gl_Position = vec4(finalPos, 0.0, 1.0);
    FragColor = aColor;
    TexCoord = aPos;
}
