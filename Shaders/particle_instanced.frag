#version 330 core
in float vLife;
out vec4 FragColor;
void main(){
 // 연기 색상: life 감소하면 알파도 감소, 더 진한 연기 효과
 float alpha = clamp(vLife, 0.0, 1.0);
 // 더 진한 연기 색상으로 변경: 어두운 회색에서 밝은 회색으로
 vec3 baseColor = mix(vec3(0.1, 0.1, 0.1), vec3(0.4, 0.4, 0.4), vLife); // 더 어둡고 짙은 연기
 // 알파 값도 더 높여서 더 진하게 보이도록 (0.6 -> 0.85)
 FragColor = vec4(baseColor, alpha * 0.85);
}
