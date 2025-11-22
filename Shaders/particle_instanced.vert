#version 330 core
layout(location=0) in vec3 aPos; // cube local
layout(location=4) in vec4 iData; // xyz position, w scale
uniform mat4 uView;
uniform mat4 uProj;
out float vLife; // encoded from scale
void main(){
 float scale = iData.w;
 vec3 worldPos = iData.xyz + aPos * scale;
 gl_Position = uProj * uView * vec4(worldPos,1.0);
 // 더 큰 파티클과 더 긴 수명에 맞춰 라이프 계산 조정
 // 스케일 범위를 2.4f 기준으로 조정 (더 큰 파티클 크기에 맞춤)
 vLife = clamp(scale / 3.6, 0.0, 1.0); // 1.2에서 3.6으로 증가 (더 큰 파티클에 맞춤)
}
