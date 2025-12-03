#include "Ground.h"

Ground::Ground()
	: VAO(0), VBO(0), EBO(0), textureID(0), size(1000.0f), gridResolution(100), groundTexture("T_RockyGround_A.png"), 
    normalMap("T_RockyGround_NA.png"),
    heightMap("heightMap.png"), heightMapWidth(0), heightMapHeight(0), numStrips(0), numTrisPerStrip(0), heightScale(21.5f)
{
}

Ground::~Ground()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (textureID != 0) glDeleteTextures(1, &textureID);
}

void Ground::Initialize()
{
	groundTexture.LoadTexture();
	normalMap.LoadTexture();
	
 
    int nrChannels;
    unsigned char* data = heightMap.GetImageData(&heightMapWidth, &heightMapHeight, &nrChannels);
    
    if (!data)
    {
        std::cout << "Failed to load heightmap data, creating flat terrain" << std::endl;
        // Fall back to flat terrain generation
        CreateFlatTerrain();
        return;
    }
    
    std::cout << "Loaded heightmap of size " << heightMapHeight << " x " << heightMapWidth << std::endl;
    
    vertices.clear();
    indices.clear();

    float yScale = (64.0f / 256.0f) * heightScale;
    float yShift = 16.0f;
    int rez = 1;
    unsigned bytePerPixel = nrChannels;
    
    std::cout << "Using height scale: " << heightScale << " (yScale: " << yScale << ")" << std::endl;
    
    for(int i = 0; i < heightMapHeight; i++)
    {
        for(int j = 0; j < heightMapWidth; j++)
        {
            unsigned char* pixelOffset = data + (j + heightMapWidth * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            float xPos = -heightMapHeight / 2.0f + heightMapHeight * i / (float)heightMapHeight;
            float yPos = (int)y * yScale - yShift;
            float zPos = -heightMapWidth / 2.0f + heightMapWidth * j / (float)heightMapWidth;
            
            // UV coordinates
            float u = (float)j / heightMapWidth * 10.0f;  
            float v = (float)i / heightMapHeight * 10.0f;

            // Position (3)
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            // UV (2)
            vertices.push_back(u);
            vertices.push_back(v);

            // Normal (3) - will be calculated properly later, for now up vector
            vertices.push_back(0.0f);
            vertices.push_back(-1.0f);
            vertices.push_back(0.0f);

            // Tangent (3) - X axis direction
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }
    
    std::cout << "Loaded " << vertices.size() / 11 << " vertices" << std::endl;

    // Generate indices using triangle strips
    for (int i = heightMapHeight - 2; i >= 0; i -= rez)
    {
        for (int j = heightMapWidth - 1; j >= 0; j -= rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                // 역순 정점에 맞게 인덱스 계산
                int vertexIndex = (heightMapHeight - 1 - (i + k * rez)) * heightMapWidth + (heightMapWidth - 1 - j);
                indices.push_back(vertexIndex);
            }
        }
    }
    
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    numStrips = (heightMapHeight - 1) / rez;
    numTrisPerStrip = (heightMapWidth / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    // Free the image data
    stbi_image_free(data);

    // VAO/VBO/EBO setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    const GLsizei stride = 11 * sizeof(GLfloat);

    // location 0: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // location 1: UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // location 2: normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // location 3: tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    std::cout << "Ground 초기화 완료 - Heightmap terrain created" << std::endl;
}

void Ground::CreateFlatTerrain()
{
    // Fallback: create flat terrain
    float halfSize = size / 2.0f;
    float step = size / gridResolution;
   
    vertices.clear();
    indices.clear();

    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    // Generate vertices
    for (int z = 0; z <= gridResolution; ++z)
    {
        for (int x = 0; x <= gridResolution; ++x)
        {
            float xPos = -halfSize + x * step;
            float zPos = -halfSize + z * step;
            float yPos = 4.0f;

            // UV coordinates (tiling)
            float u = (float)x / gridResolution * 10.0f;
            float v = (float)z / gridResolution * 10.0f;

            // Position (3)
            vertices.push_back(xPos * 10);
            vertices.push_back((int)yPos * yScale - yShift);
            vertices.push_back(zPos * 10);

            // UV (2)
            vertices.push_back(u);
            vertices.push_back(v);

            // Normal (3)
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);

            // Tangent (3)
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    // Generate indices
    for (int z = 0; z < gridResolution; ++z)
    {
        for (int x = 0; x < gridResolution; ++x)
        {
            int topLeft = z * (gridResolution + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridResolution + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // VAO/VBO/EBO setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    const GLsizei stride = 11 * sizeof(GLfloat);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    std::cout << "Ground 초기화 완료 - 크기: " << size << ", 정점: " << vertices.size() / 11
        << ", 인덱스: " << indices.size() << std::endl;
}

void Ground::Update()
{
    // Add update logic if needed
}

void Ground::Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj)
{
    glUseProgram(shaderProgramID);

    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
    GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");
    GLint useNormalMapLoc = glGetUniformLocation(shaderProgramID, "useNormalMap");
    GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);

	
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(XZScale, 1.0f, XZScale));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    
    // Texture settings
    glUniform1f(useTextureLoc, 1.0f);
    glUniform1f(alphaValueLoc, 1.0f);
    glUniform1i(useNormalMapLoc, 1);


	groundTexture.UseTexture(0);
	normalMap.UseTexture(1);

    // Draw terrain
    glBindVertexArray(VAO);
    
    if (numStrips > 0 && numTrisPerStrip > 0)
    {
        // Draw using triangle strips (heightmap-based terrain)
        for(unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,
                           numTrisPerStrip + 2,
                           GL_UNSIGNED_INT,
                           (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
        }
    }
    else
    {
        // Draw using regular triangles (flat terrain)
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}

void Ground::ControlHeightmap(float scale)
{
    // heightScale 업데이트 후 vertices y 좌표에 스케일 값 적용
    for (size_t i = 0; i < vertices.size() / 11; ++i)
    {
        vertices[i * 11 + 1] = vertices[i * 11 + 1] / heightScale * scale;
    } 
    heightScale = scale; // 현재 스케일 값 저장


    // 기존 VAO 바인딩 후 버퍼 데이터만 업데이트
    glBindVertexArray(VAO);

    // VBO 업데이트 (GL_DYNAMIC_DRAW 사용)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

    // EBO는 변경되지 않으므로 업데이트 불필요

    glBindVertexArray(0);
}

float Ground::GetHeightAt(float worldX, float worldZ) const
{
    if (vertices.empty() || heightMapWidth == 0 || heightMapHeight == 0)
    {
     return -2.0f; // Return ground level if heightmap not loaded (accounting for translation)
    }

    // Terrain transformation in Render():
    // 1. translate(0, -2, 0)
 // 2. scale(10, 1, 10)
    // 
    // In Initialize(), terrain vertices are created as:
    // xPos = -heightMapHeight/2 + i  (ranges from -heightMapHeight/2 to heightMapHeight/2)
    // zPos = -heightMapWidth/2 + j   (ranges from -heightMapWidth/2 to heightMapWidth/2)
    //
    // After scaling by 10:
    // World X ranges: [-heightMapHeight*5, heightMapHeight*5]
    // World Z ranges: [-heightMapWidth*5, heightMapWidth*5]
    
    // Reverse the scaling (divide by 10)
    float localX = worldX / XZScale;
    float localZ = worldZ / XZScale;
    
    // Convert from local space to grid indices
    // localX ranges from -heightMapHeight/2 to +heightMapHeight/2
    // We need to map this to [0, heightMapHeight-1]
    float gridI = localX + (heightMapHeight / 2.0f);
    float gridJ = localZ + (heightMapWidth / 2.0f);
    
    // Check bounds
    if (gridI < 0.0f || gridI >= heightMapHeight - 1 || 
      gridJ < 0.0f || gridJ >= heightMapWidth - 1)
    {
        return -2.0f; // Outside terrain bounds, return ground level
    }
    
    // Get integer indices for the four surrounding vertices
    int i0 = (int)std::floor(gridI);
    int j0 = (int)std::floor(gridJ);
    int i1 = i0 + 1;
    int j1 = j0 + 1;
    
    // Get fractional parts for interpolation
    float fi = gridI - i0;
    float fj = gridJ - j0;
    
  
    auto getVertexHeight = [this](int i, int j) -> float {
        if (i < 0 || i >= heightMapHeight || j < 0 || j >= heightMapWidth)
            return 0.0f;
        int index = (i * heightMapWidth + j) * 11 + 1; // +1 for Y coordinate
        if (index >= 0 && index < (int)vertices.size())
            return vertices[index];
        return 0.0f;
        };
    
    float h00 = getVertexHeight(i0, j0);
    float h10 = getVertexHeight(i1, j0);
    float h01 = getVertexHeight(i0, j1);
    float h11 = getVertexHeight(i1, j1);
    
    // Bilinear interpolation
    float h0 = h00 * (1.0f - fi) + h10 * fi;
    float h1 = h01 * (1.0f - fi) + h11 * fi;
    float height = h0 * (1.0f - fj) + h1 * fj;
    
    // Account for the -2.0f Y translation in Render()
    return height - 2.0f;
}
