#pragma once
#include "CommonInclude.h"
#include "ParticleSystem.h"

class Missile
{
public:
    Missile();
    ~Missile();

    void Initialize();
    void Update(float deltaTime);
    void Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj);
    void RenderMissileLight(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj);

    // 위치 및 방향 설정
    void SetPosition(const glm::vec3& position);
    void SetDirection(const glm::vec3& direction);
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetDirection() const { return direction; }

    // 발사 관련
    void Launch(const glm::vec3& startPos, const glm::vec3& direction);
    bool IsActive() const { return isActive; }
    void Deactivate(); // changed to function so explosion can be triggered
    
    // Return true when missile is inactive and its particle system has no live particles
    bool IsFinished() const;
    
    // 조명 관련
    glm::vec3 GetLightPosition() const;
    glm::vec3 GetLightColor() const { return lightColor; }
    float GetLightIntensity() const { return lightIntensity; }

    // Set ground reference for terrain collision
    void SetGround(class Ground* ground) { mGround = ground; }

private:
    void CreateCubeGeometry();
    void SetupBuffers();
    void CreateLightGeometry();
    void SetupLightBuffers();

    // Explosion helper
    void ExplodeAt(const glm::vec3& pos);

    // OpenGL 버퍼
    GLuint VAO, VBO, EBO;
    GLuint lightVAO, lightVBO, lightEBO;
    
    // 기하학적 데이터
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> lightVertices;
    std::vector<GLuint> lightIndices;
    
    // 육면체 크기 (가로 5.0, 세로 20.0, 높이 5.0)
    float width = 3.0f;
    float height = 10.0f;
    float depth = 3.0f;
    
    // 미사일 상태
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;
    float speed = 1000.0f;  // 속도 증가
    bool isActive = false;

    // 색상 - 노란색으로 변경
    glm::vec3 missileColor = glm::vec3(1.0f, 1.0f, 0.0f); // 노란색
    
    // 조명 관련 - 더 밝고 잘 보이는 값들로 수정
    glm::vec3 lightColor = glm::vec3(1.0f, 0.8f, 0.0f); // 노란색-주황색 혼합
    float lightIntensity = 1.0f; // 2.0f에서 4.0f로 증가
    float lightOffset = -8.0f; // 미사일 뒤쪽 오프셋 (더 가까이)
    float lightPulseTimer = 0.0f;
    
    // 파티클 시스템
    ParticleSystem smokeTrail;
    float particleEmissionTimer;
    const float particleEmissionRate = 0.02f; // 50 파티클/초
    float trailSpacing = 2.5f; // 궤적 연기 간격 (선형 배치용)

    // Terrain reference for collision detection
    class Ground* mGround;
};

