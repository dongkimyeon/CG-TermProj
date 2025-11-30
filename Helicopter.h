#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"
#include "Missile.h"
#include "CannonBullet.h"

class Camera; // forward declaration

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
	float GetCannonPitch() const { return cannonPitch; }
	float GetCannonYaw() const { return cannonYaw; }

    
    // 무기 관련
    void FireMissile();
    // 오버로드: 지정한 위치/방향으로 미사일 발사
    void FireMissile(const glm::vec3& launchPos, const glm::vec3& launchDir);
    void UpdateMissiles(float deltaTime);
    void RenderMissiles(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
    
    // 카메라 기반 미사일 발사
    void FireMissileFromCamera(const Camera* camera, float crosshairDistance);
 
    // 기관포 발사 관련

    void FireCannon();
    // 새 오버로드: 월드 방향으로 기관포 발사
    void FireCannon(const glm::vec3& dir);
    void UpdateCannonBullets(float dt);
    void RenderCannonBullets(const glm::mat4& view, const glm::mat4& proj);
    
    // Setter
    void SetDebugRotation(float x, float y, float z) {
        debugRotationX = x;
        debugRotationY = y;
        debugRotationZ = z;
    }
    void SetYaw(float yaw) { yRotation = yaw; }
	void SetCannonPitch(float pitch) { cannonPitch = pitch; }
	void SetCannonYaw(float yaw) { cannonYaw = yaw; }
 // 위치 관련 getter/setter
 glm::vec3& GetCannonOffset() { return cannonOffset; }
 void SetCannonOffset(const glm::vec3& offset) { cannonOffset = offset; }
	glm::vec3& GetCannonHingePos() { return cannonHingePos; }
	void SetCannonHingePos(const glm::vec3& pos) { cannonHingePos = pos; } 
 glm::vec3 GetCannonWorldPosition() const;

 // 성능 튜닝 getter/setter
 float& GetMaxSpeed() { return maxSpeed; }
 float& GetAccelerationRate() { return accelerationRate; }
 float& GetMaxTiltAngle() { return maxTiltAngle; }
 
private:
 void UpdatePhysics(float deltaTime);
 void UpdateOrientation(float deltaTime);
 void UpdateMissilePositions();
 glm::vec3 GetMissileAttachmentPosition() const;
 glm::mat4 GetHelicopterTransform() const;
 void InitBuffers(); // 버퍼 초기화
 void CleanupBuffers(); // 버퍼 정리
 
 // 모델
 FBXModel bodyModel;
 FBXModel bladeModel;
 FBXModel tailModel;
 FBXModel CannonModel;
 
 // GL 버퍼
 GLuint vaoBody, vboBody, eboBody;
 GLuint vaoBlade, vboBlade, eboBlade;
 GLuint vaoTail, vboTail, eboTail;
	GLuint vaoCannon, vboCannon, eboCannon;
 
 // 상태
 glm::vec3 position;
 glm::vec3 velocity;
 glm::vec3 acceleration;
 
 glm::vec3 forward;
 glm::vec3 up;
 glm::vec3 right;
 
 float yRotation; // Yaw
 
 // 물리 관련
 float targetPitch;
 float targetRoll;
 float currentPitch;
 float currentRoll;
 float tiltSpeed;
 
 // 블레이드
 float mainBladeRotation;
 float mainBladeSpeed;
 float tailBladeRotation;
 float tailBladeSpeed;
 
 // 물리/조정
 float gravity;
 float maxSpeed;
 float accelerationRate;
 float drag;
 float maxTiltAngle;
 float liftForce;
 float maxLiftForce;
 
 // 디버그
 float debugRotationX;
 float debugRotationY;
 float debugRotationZ;
 
 // 기관포 관련
 glm::vec3 cannonOffset = glm::vec3(0.0f, -5.0f,15.0f); // 초기값
	float cannonPitch =0.0f; // 피치
 float cannonYaw =0.0f; // 요
 glm::vec3 cannonHingePos = glm::vec3(-2.2f,0.0f,0.0f);

 // 미사일
 std::vector<Missile*> missiles;
 std::vector<Missile*> attachedMissiles; // 장착된 미사일
 float missileAttachmentOffset = -8.0f; // 장착 높이
 int maxMissiles =30; // 최대 장착
 float missileSpacing =3.0f; // 간격

 // 기관포 총알
 std::vector<CannonBullet*> cannonBullets;
};
