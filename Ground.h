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
    
    // Get terrain height at world position (x, z)
    float GetHeightAt(float x, float z) const;
   
    // Returns the half-extents of the terrain in world space (radius from center)
    // Terrain ranges from -GetWorldSize().x to +GetWorldSize().x on X axis
    // and from -GetWorldSize().y to +GetWorldSize().y on Z axis
    //
    // Calculation: (heightMapSize / 2) * XZScale
    // Example: If heightMap is 260x260 and XZScale = 50:
    //   X range: ¡¾(260/2 * 50) = ¡¾6500
    //   Z range: ¡¾(260/2 * 50) = ¡¾6500
    glm::vec2 GetWorldSize() const {
        // Terrain is created from -heightMapHeight/2 to +heightMapHeight/2 (before scaling)
        // After XZScale scaling, the range becomes:
        // X: from -(heightMapHeight/2 * XZScale) to +(heightMapHeight/2 * XZScale)
        // Z: from -(heightMapWidth/2 * XZScale) to +(heightMapWidth/2 * XZScale)
        return glm::vec2(
            (heightMapHeight / 2.0f) * XZScale,  // Half-width on X axis
            (heightMapWidth / 2.0f) * XZScale    // Half-width on Z axis
        );
    }
    
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

	// XZ scaling factor applied to terrain coordinates
	// Current heightmap: 257x257
	// Desired terrain range: ¡¾6500
	// Calculation: 6500 / (257/2) = 6500 / 128.5 = 50.59
	// Result: (257/2) * 50.59 ? 6500
	const float XZScale = 50.59f;  // Adjusted for 257x257 heightmap to give ¡¾6500 range
};