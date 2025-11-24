#version 330 core
out vec4 FragColor;

uniform vec3 crosshairColor = vec3(0.0, 1.0, 0.0); // ±âº» ³ì»ö

void main()
{
    FragColor = vec4(crosshairColor, 1.0);
}