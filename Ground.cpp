#include "Ground.h"

Ground::Ground()
	: VAO(0), VBO(0), EBO(0), textureID(0), size(1000.0f), gridResolution(100), groundTexture("T_RockyGround_A.png"), 
    normalMap("T_RockyGround_NA.png"),
    heightMap("heightMap.png"), heightMapWidth(0), heightMapHeight(0), numStrips(0), numTrisPerStrip(0), heightScale(1.0f)
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
	
    // Load heightmap image for terrain generation using stb_image
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
            vertices.push_back(1.0f);
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
	model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f)); 
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
