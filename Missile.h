#pragma once
#include "CommonInclude.h"

class Missile
{
public:
    Missile();
    ~Missile();

    void Initialize();
    void Update(float deltaTime);
    void Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj);

    // 위치 및 방향 설정
    void SetPosition(const glm::vec3& position);
    void SetDirection(const glm::vec3& direction);
    glm::vec3 GetPosition() const { return position; }

    // 발사 관련
    void Launch(const glm::vec3& startPos, const glm::vec3& direction);
    bool IsActive() const { return isActive; }
    void Deactivate() { isActive = false; }

private:
    void CreateCubeGeometry();
    void SetupBuffers();

    // OpenGL 버퍼
    GLuint VAO, VBO, EBO;
    
    // 기하학적 데이터
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    
    // 육면체 크기 (가로 5.0, 세로 20.0, 높이 5.0)
    float width = 5.0f;
    float height = 20.0f;
    float depth = 5.0f;
    
    // 미사일 상태
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;
    float speed = 100.0f;  // 속도 증가
    bool isActive = false;
    
    // 색상 - 노란색으로 변경
    glm::vec3 missileColor = glm::vec3(1.0f, 1.0f, 0.0f); // 노란색
};

