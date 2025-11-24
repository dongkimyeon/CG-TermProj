#version 330 core
in vec4 vColor;
in vec2 vUV;
out vec4 FragColor;
void main(){
 // create soft circular falloff
 float dist = length(vUV - vec2(0.5));
 float alpha = smoothstep(0.5,0.0, dist);
 vec4 col = vColor;
 col.a *= alpha;
 // output premultiplied alpha to blend nicely
 FragColor = vec4(col.rgb * col.a, col.a);
}
