#include "Camera.h"

Camera::Camera()
    : position(glm::vec3(75.0f, 75.0f, 75.0f))
    , target(glm::vec3(0.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , radius(0.0f)
    , distance(150.0f)
    , height(50.0f)
    , rotationSpeed(0.001f)
    , targetCameraXAngle(0.0f)
    , targetCameraYAngle(0.0f)
{
}

void Camera::Initialize(const glm::vec3& pos, float dist, float h)
{
    position = pos;
    distance = dist;
    height = h;
    
    radius = glm::length(position);
    float r_xz_init = glm::sqrt(position.x * position.x + position.z * position.z);
    float cameraAngle = atan2(position.z, position.x);
    float cameraYAngle = atan2(position.y, r_xz_init);
    targetCameraXAngle = cameraAngle;
    targetCameraYAngle = cameraYAngle;
    
    std::cout << "카메라 초기 설정 완료 - 반지름: " << radius 
              << ", 각도: " << glm::degrees(cameraAngle) << "도" << std::endl;
}

void Camera::Update(float deltaTime, const glm::vec3& targetPosition, const glm::vec3& targetUp, const glm::vec3& targetForward)
{
    // 카메라가 타겟을 따라가도록 오프셋 계산
    glm::vec3 cameraOffset = targetForward * distance + targetUp * height;
    position = targetPosition + cameraOffset;
    
    // 카메라 목표 지점 (타겟 위쪽을 바라봄)
    target = targetPosition + targetUp * 10.0f;
    up = targetUp;
}

void Camera::ProcessMouseDrag(int deltaX, int deltaY, float& heliYawRotation)
{
    // 마우스 좌우 움직임으로 헬기 Yaw 회전 제어
    heliYawRotation += deltaX * rotationSpeed * 100.0f;
    
    // 카메라는 헬기를 따라가도록 설정
    targetCameraXAngle -= deltaX * rotationSpeed;
    
    // 카메라 고도각 업데이트
    targetCameraYAngle += deltaY * rotationSpeed;
    
    // 고도각 제한 (-89도 ~ 89도)
    const float maxAngle = glm::radians(89.0f);
    if (targetCameraYAngle > maxAngle) targetCameraYAngle = maxAngle;
    if (targetCameraYAngle < -maxAngle) targetCameraYAngle = -maxAngle;
}

void Camera::Zoom(int direction)
{
    float zoomSpeed = 5.0f;
    if (direction > 0)
    {
        radius -= zoomSpeed;
        if (radius < 2.0f)
            radius = 2.0f;
    }
    else
    {
        radius += zoomSpeed;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspect, float fov) const
{
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}
