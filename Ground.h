#pragma once
#include "CommonInclude.h"
#include "Texture.h"

class Ground
{
public:
    Ground();
    ~Ground();

    void Initialize();
    void Update();
    void Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj);
    
    void ControlHeightmap(float scale);
private:
    void CreateFlatTerrain(); // Fallback method for flat terrain
    
    GLuint VAO, VBO, EBO;
    GLuint textureID;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
	Texture groundTexture;
    Texture normalMap;
	Texture heightMap;
    float size;  
    int gridResolution;
    
    // Heightmap data
    int heightMapWidth;
    int heightMapHeight;
    int numStrips;
    int numTrisPerStrip;
	float heightScale;
};