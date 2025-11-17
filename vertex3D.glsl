#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;



uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 evePos, lightDir;


out vec2 UV;

out vec3 v_lightTs, v_viewTS;


void main() {
   
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    UV = aUV;

    // view vector ±¸ÇÏ·Á°í worldPos ±¸ÇÏ±â
	vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
	// v_view = normalize(eyePos - worldPos);

	// ÅºÁ¨Æ® °ø°£ ³ë¸Ö ¸ÅÇÎ À§ÇØ TBN ±¸ÇÏ±â
	vec3 Nor = normalize(transpose(inverse(mat3(model))) * aNormal);
	vec3 Tan = normalize(transpose(inverse(mat3(model))) * aTangent);
	vec3 Bit = cross(Nor, Tan);
	// ÅºÁ¨Æ® °ø°£ º¯È¯ Çà·Ä
	mat3 tbnMat = transpose(mat3(Tan, Bit, Nor));

	// ºû º¤ÅÍ, ºä º¤ÅÍ¸¦ ÅºÁ¨Æ® °ø°£À¸·Î º¯È¯
	v_lightTs = tbnMat * normalize(lightDir);
	v_viewTS = tbnMat * normalize(evePos - worldPos);
}