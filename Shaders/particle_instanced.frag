#version 330 core
in float vLife;
out vec4 FragColor;
void main(){
 // 연기 색상: life 감소하면 알파도 감소, 약간 어두워짐
 float alpha = clamp(vLife,0.0,1.0);
 vec3 baseColor = mix(vec3(0.2), vec3(0.6), vLife); // 시작 밝고 끝 어둡게
 FragColor = vec4(baseColor, alpha*0.6);
}
