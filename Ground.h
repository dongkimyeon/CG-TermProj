#pragma once
#include "CommonInclude.h"

class Ground
{
public:
    Ground();
    ~Ground();

    void Initialize();
    void Update();
    void Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj);

private:
    GLuint VAO, VBO, EBO;
    GLuint textureID;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float size;  
    int gridResolution; 
};