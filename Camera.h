#pragma once
#include "CommonInclude.h"

class Camera {
public:
    Camera();
    
    // 초기화
    void Initialize(const glm::vec3& position, float distance, float height);
    
    // 업데이트
    void Update(float deltaTime, const glm::vec3& targetPosition, const glm::vec3& targetUp, const glm::vec3& targetForward);
    
    // 마우스 입력 처리
    void ProcessMouseDrag(int deltaX, int deltaY, float& heliYawRotation);
    void Zoom(int direction);
    
    // Getter
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetTarget() const { return target; }
    glm::vec3 GetUp() const { return up; }
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect, float fov = 100.0f) const;
    
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    float radius;
    float distance;      // 타겟으로부터의 거리
    float height;        // 타겟 위쪽으로의 높이
    float rotationSpeed;
    
    float targetCameraXAngle;
    float targetCameraYAngle;
};
