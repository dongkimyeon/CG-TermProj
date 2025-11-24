#version 330 core
layout(location = 0) in vec3 aPos; // quad local (x,y,0)
layout(location = 1) in vec2 aUV;
layout(location = 4) in vec4 iData; // xyz position, w scale
layout(location = 5) in vec4 iColor; // rgba per-instance
uniform mat4 uView;
uniform mat4 uProj;
out vec4 vColor; // pass color to fragment
out vec2 vUV;
void main(){
 float scale = iData.w;
 // extract camera right and up vectors in world space (inverse rotation of view)
 mat3 invView = transpose(mat3(uView));
 vec3 right = invView[0];
 vec3 up = invView[1];
 // aPos.xy define quad in [-0.5,0.5]
 vec3 worldPos = iData.xyz + (right * aPos.x + up * aPos.y) * scale;
 gl_Position = uProj * uView * vec4(worldPos,1.0);
 vColor = iColor;
 vUV = aUV;
}
