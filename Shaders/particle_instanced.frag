#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main(){
 // 인스턴스 색상을 직접 사용하고, 프리멀티플라이드 알파는 여기서 필요하지 않음
 FragColor = vColor;
}
