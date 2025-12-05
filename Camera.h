#pragma once
#include "CommonInclude.h"

class Camera {
public:
    Camera();

    // 초기화
    void Initialize(const glm::vec3& position, float distance, float height);

    
    void Update(float deltaTime, const glm::vec3& targetPosition, const glm::vec3& targetUp, const glm::vec3& targetForward, float helicopterPitch = 0.0f, float helicopterRoll = 0.0f, const glm::vec3& cannonWorldPos = glm::vec3(0.0f));

    // 마우스 입력 처리
    void ProcessMouseDrag(int deltaX, int deltaY, float& heliYawRotation, float& heliCannonYaw, float& heliCannonPitch);
    void Zoom(int direction);

    // Getter
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetTarget() const { return target; }
    glm::vec3 GetUp() const { return up; }
    float GetDistance() const { return distance; }
    int GetCameraMode() const { return cameraMode;  }
	glm::vec3& GetGunnerOffset() { return gunnerOffset; }
    // Setter
    void SetCameraMode(int mode) { 
        targetCameraXAngle = 0.0f;
        targetCameraYAngle = 0.0f;
        cameraMode = mode; 
    }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetHeight(float h) { height = h; }
    void SetDistance(float d) { distance = d; }
	void SetGunnerOffset(const glm::vec3& offset) { gunnerOffset = offset; }
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect, float fov = 100.0f) const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

	glm::vec3 gunnerOffset; // 기관포 뷰 오프셋
    float radius;
    float distance;      // 타겟으로부터의 거리
    float height;        // 타겟 위쪽으로의 높이
    float rotationSpeed;

    int cameraMode = 0; // 0: 3인칭 1: 콕핏뷰 2: 기관포 사수뷰 

    float targetCameraXAngle;
    float targetCameraYAngle;

    float cameraYawAngle;
    float yawFollowSpeed;
};