#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"

class Helicopter {
public:
    Helicopter();
    ~Helicopter();
    
    // 초기화
    void Initialize();
    void LoadModels(GLuint vaoBody, GLuint vboBody, GLuint eboBody,
                    GLuint vaoBlade, GLuint vboBlade, GLuint eboBlade,
                    GLuint vaoTail, GLuint vboTail, GLuint eboTail);
    
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
    
    // Setter (디버그용)
    void SetDebugRotation(float x, float y, float z) {
        debugRotationX = x;
        debugRotationY = y;
        debugRotationZ = z;
    }
    void SetYaw(float yaw) { yRotation = yaw; }
    
    // 물리 파라미터 접근
    float& GetMaxSpeed() { return maxSpeed; }
    float& GetAccelerationRate() { return accelerationRate; }
    float& GetMaxTiltAngle() { return maxTiltAngle; }
    
private:
    void UpdatePhysics(float deltaTime);
    void UpdateOrientation(float deltaTime);
    void RenderModel(const FBXModel& model, GLuint vao, const glm::mat4& transform, 
                     GLuint shaderID, bool wireframe, float alpha);
    
    // 모델
    FBXModel bodyModel;
    FBXModel bladeModel;
    FBXModel tailModel;
    
    // 버퍼 ID
    GLuint vaoBody, vboBody, eboBody;
    GLuint vaoBlade, vboBlade, eboBlade;
    GLuint vaoTail, vboTail, eboTail;
    
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
};
