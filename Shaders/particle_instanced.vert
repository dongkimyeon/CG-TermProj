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
 // scale (0~0.8) -> life (approx) : life = scale/0.8
 vLife = scale /0.8;
}
