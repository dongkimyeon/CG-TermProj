#include "Ground.h"

Ground::Ground()
	: VAO(0), VBO(0), EBO(0), textureID(0), size(1000.0f), gridResolution(100), groundTexture("T_RockyGround_A.png"), normalMap("T_RockyGround_NA.png")
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

    // 정점 데이터 생성 (위치(3) + UV(2) + 노멀(3) + 탄젠트(3) = 11 floats)
    float halfSize = size / 2.0f;
    float step = size / gridResolution;

    vertices.clear();
    indices.clear();

    // 정점 생성
    for (int z = 0; z <= gridResolution; ++z)
    {
        for (int x = 0; x <= gridResolution; ++x)
        {
            float xPos = -halfSize + x * step;
            float zPos = -halfSize + z * step;
            float yPos = 0.0f;  // 바닥은 y=0

            // UV 좌표 (타일링)
            float u = (float)x / gridResolution * 10.0f;  // 10배 타일링
            float v = (float)z / gridResolution * 10.0f;

            // 위치 (3)
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            // UV (2)
            vertices.push_back(u);
            vertices.push_back(v);

            // 노멀 (3) - 위쪽 향함
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            // 탄젠트 (3) - X축 방향
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    // 인덱스 생성
    for (int z = 0; z < gridResolution; ++z)
    {
        for (int x = 0; x < gridResolution; ++x)
        {
            int topLeft = z * (gridResolution + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridResolution + 1) + x;
            int bottomRight = bottomLeft + 1;

            // 첫 번째 삼각형
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // 두 번째 삼각형
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // VAO/VBO/EBO 생성
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

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

    std::cout << "Ground 초기화 완료 - 크기: " << size << ", 정점: " << vertices.size() / 11
        << ", 인덱스: " << indices.size() << std::endl;
}

void Ground::Update()
{
    // 필요시 업데이트 로직 추가
}

void Ground::Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj)
{
    glUseProgram(shaderProgramID);

    // 유니폼 위치 가져오기
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
    GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");
    GLint useNormalMapLoc = glGetUniformLocation(shaderProgramID, "useNormalMap");

    // 모델 행렬 (바닥은 변환 없음)
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    

    
	groundTexture.UseTexture(0);
	normalMap.UseTexture(1);


    glUniform1f(alphaValueLoc, 1.0f);
    glUniform1i(useNormalMapLoc, 1);


    // 바닥 그리기
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}