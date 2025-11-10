#version 330 core
in vec3 ourColor;
in vec2 UV;
out vec4 FragColor;
uniform sampler2D ourTexture;
uniform bool useVertexColor;
uniform float alphaValue; // <-- 알파값 유니폼 추가

void main() {

    if (useVertexColor)
    {
        FragColor = vec4(ourColor, 1.0); // 정점 색상 사용
    }
    else
    {
        vec4 texColor = texture(ourTexture, UV); // 텍스처 사용
        FragColor = vec4(texColor.rgb, texColor.a * alphaValue); // 알파값 적용
    }

}