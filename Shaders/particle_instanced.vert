#version 330 core
layout(location = 0) in vec3 aPos; // cube local
layout(location = 4) in vec4 iData; // xyz position, w scale
layout(location = 5) in vec4 iColor; // rgba per-instance
uniform mat4 uView;
uniform mat4 uProj;
out vec4 vColor; // pass color to fragment
void main(){
 float scale = iData.w;
 vec3 worldPos = iData.xyz + aPos * scale;
 gl_Position = uProj * uView * vec4(worldPos,1.0);
 vColor = iColor;
}
