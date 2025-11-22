#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"
#include "Missile.h"

class Helicopter {
public:
    Helicopter();
    ~Helicopter();
    
    // 초기화
    void Initialize();
    void LoadModels();
    
    // 업데이트
    void Update(float deltaTime);
    void ProcessInput(float deltaTime);
    
    // 렌더링
    void Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale);
    
    // Getter
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetVelocity() const { return velocity; }
    glm::vec3 GetForward() const { return forward; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetRight() const { return right; }
    float GetYaw() const { return yRotation; }
    float GetPitch() const { return currentPitch; }
    float GetRoll() const { return currentRoll; }
    
    // 미사일 관련
    void FireMissile();
    void UpdateMissiles(float deltaTime);
    void RenderMissiles(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
    
    // Setter (디버그용)
    void SetDebugRotation(float x, float y, float z) {
        debugRotationX = x;
        debugRotationY = y;
        debugRotationZ = z;
    }
    void SetYaw(float yaw) { yRotation = yaw; }
    
    // 기관포 오프셋 getter/setter
    glm::vec3& GetCannonOffset() { return cannonOffset; }
    void SetCannonOffset(const glm::vec3& offset) { cannonOffset = offset; }
    
    // 물리 파라미터 접근
    float& GetMaxSpeed() { return maxSpeed; }
    float& GetAccelerationRate() { return accelerationRate; }
    float& GetMaxTiltAngle() { return maxTiltAngle; }
    
private:
    void UpdatePhysics(float deltaTime);
    void UpdateOrientation(float deltaTime);
    void UpdateMissilePositions();
    void RenderModel(const FBXModel& model, GLuint vao, const glm::mat4& transform, 
                     GLuint shaderID, bool wireframe, float alpha);
    glm::vec3 GetMissileAttachmentPosition() const;
    glm::mat4 GetHelicopterTransform() const;
    void InitBuffers();  // 버퍼 초기화 메서드 추가
    void CleanupBuffers();  // 버퍼 정리 메서드 추가
    
    // 모델
    FBXModel bodyModel;
    FBXModel bladeModel;
    FBXModel tailModel;
    FBXModel CannonModel;
    
    // 버퍼 ID
    GLuint vaoBody, vboBody, eboBody;
    GLuint vaoBlade, vboBlade, eboBlade;
    GLuint vaoTail, vboTail, eboTail;
	GLuint vaoCannon, vboCannon, eboCannon;
    
    // 위치 및 방향
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
    
    float yRotation;  // Yaw
    
    // 기울기
    float targetPitch;
    float targetRoll;
    float currentPitch;
    float currentRoll;
    float tiltSpeed;
    
    // 블레이드 회전
    float mainBladeRotation;
    float mainBladeSpeed;
    float tailBladeRotation;
    float tailBladeSpeed;
    
    // 물리 파라미터
    float gravity;
    float maxSpeed;
    float accelerationRate;
    float drag;
    float maxTiltAngle;
    float liftForce;
    float maxLiftForce;
    
    // 디버그 회전
    float debugRotationX;
    float debugRotationY;
    float debugRotationZ;
    
    // 기관포 오프셋 
    glm::vec3 cannonOffset = glm::vec3(0.0f, -5.0f, 15.0f); // 초기값: 바디 앞쪽 약간 아래
	float cannonPitch = -45.0f; // 기관포 피치 각도
    float cannonYaw = -45.0f;    // 기관포 요 각도
    // 미사일 시스템
    std::vector<Missile*> missiles;
    std::vector<Missile*> attachedMissiles;  // 헬리콥터에 붙어있는 미사일
    float missileAttachmentOffset = -8.0f;   // 헬리콥터 아래쪽 오프셋
    int maxMissiles = 30;    // 최대 미사일 개수
    float missileSpacing = 3.0f;           // 미사일 간 간격
};
