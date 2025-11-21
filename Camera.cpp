#include "Camera.h"

Camera::Camera()
    : position(glm::vec3(75.0f, 75.0f, 75.0f))
    , target(glm::vec3(0.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , radius(0.0f)
    , distance(150.0f)
    , height(50.0f)
    , rotationSpeed(0.001f)
    , cameraMode(0)
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

void Camera::Update(float deltaTime, const glm::vec3& targetPosition, const glm::vec3& targetUp, const glm::vec3& targetForward, float helicopterPitch, float helicopterRoll)
{
    switch (cameraMode) {
    case 0: // 3인칭 뷰
    {
        // 기존 3인칭 카메라
        glm::vec3 cameraOffset = targetForward * distance + targetUp * height;
        position = targetPosition + cameraOffset;
        target = targetPosition + targetUp * 10.0f;
        up = targetUp;
        break;
    }
    case 1: // 콕핏 1인칭 뷰 (피치/롤 완전 반영)
    {
        // Right 벡터 계산
        glm::vec3 right = glm::normalize(glm::cross(-targetForward, targetUp));

        // 조종석 기본 오프셋 (헬리콥터 로컬 좌표계)
        glm::vec3 localCockpitOffset = glm::vec3(15.0f, 5.0f, 0.0f); // forward, up, right

        // 피치 회전 매트릭스
        float pitchRad = glm::radians(helicopterPitch);
        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), pitchRad, right);

        // 롤 회전 매트릭스
        float rollRad = glm::radians(helicopterRoll);
        glm::mat4 rollRotation = glm::rotate(glm::mat4(1.0f), rollRad, -targetForward);

        // 전체 회전 매트릭스 (롤 -> 피치 순서)
        glm::mat4 rotationMatrix = pitchRotation * rollRotation;

        // 로컬 오프셋을 월드 좌표계로 변환
        glm::vec3 worldOffset = -targetForward * localCockpitOffset.x
            + targetUp * localCockpitOffset.y
            + right * localCockpitOffset.z;

        // 오프셋에 회전 적용
        glm::vec4 rotatedOffset = rotationMatrix * glm::vec4(worldOffset, 0.0f);

        // 카메라 위치 = 헬리콥터 위치 + 회전된 오프셋
        position = targetPosition + glm::vec3(rotatedOffset);

        // 전방 벡터에 회전 적용
        glm::vec4 rotatedForward = rotationMatrix * glm::vec4(-targetForward, 0.0f);

        // Up 벡터에 회전 적용
        glm::vec4 rotatedUp = rotationMatrix * glm::vec4(targetUp, 0.0f);

        // 카메라가 바라보는 목표점
        target = position + glm::vec3(rotatedForward) * 100.0f;
        up = glm::normalize(glm::vec3(rotatedUp));
        break;
    }
    case 2: // 기관포 사수 뷰
    {
        // 기관포 위치 (헬리콥터 앞쪽 아래)
        glm::vec3 gunnerOffset = -targetForward * 20.0f - targetUp * 10.0f;
        position = targetPosition + gunnerOffset;

        // 약간 아래를 향하도록
        target = targetPosition - targetForward * 100.0f - targetUp * 20.0f;
        up = targetUp;
        break;
    }
    }
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
    if (cameraMode != 0) return;  // 1인칭 모드에서는 줌 비활성화

    float zoomSpeed = 10.0f;

    if (direction > 0)  // 줌 인
    {
        distance -= zoomSpeed;
        if (distance < 10.0f)  // 최소 거리
            distance = 10.0f;
    }
    else  // 줌 아웃
    {
        distance += zoomSpeed;
        if (distance > 500.0f)  // 최대 거리
            distance = 500.0f;
    }

    // radius도 같이 조정
    radius = distance;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspect, float fov) const
{
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 10000.0f);
}