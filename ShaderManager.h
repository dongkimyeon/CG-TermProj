#pragma once
#include "CommonInclude.h"

class ShaderManager {
public:
    // 일반 셰이더 프로그램 생성
    static GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
    
    // 스카이박스 셰이더 프로그램 생성
    static GLuint CreateSkyboxShaderProgram();
    
    // 큐브맵 로드
    static GLuint LoadCubemap(const std::vector<std::string>& faces);
    
private:
    static GLuint CompileShader(const char* path, GLenum shaderType);
    static GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
};
