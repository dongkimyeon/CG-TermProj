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
	, gunnerOffset(glm::vec3(7.7f, 1.8f, 0.0f)) // 기관포 뷰 기본 오프셋
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

void Camera::Update(float deltaTime, const glm::vec3& targetPosition, const glm::vec3& targetUp, const glm::vec3& targetForward, float helicopterPitch, float helicopterRoll, const glm::vec3& cannonWorldPos)
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
        glm::mat4 rotationMatrix = rollRotation * pitchRotation;

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
        // Right 벡터 계산
        glm::vec3 right = glm::normalize(glm::cross(-targetForward, targetUp));

        // 1. 헬리콥터의 피치/롤 회전 매트릭스
        float pitchRad = glm::radians(helicopterPitch);
        glm::mat4 heliPitchRotation = glm::rotate(glm::mat4(1.0f), pitchRad, right);

        float rollRad = glm::radians(helicopterRoll);
        glm::mat4 heliRollRotation = glm::rotate(glm::mat4(1.0f), rollRad, -targetForward);

        // 헬리콥터 전체 회전 (롤 -> 피치)
        glm::mat4 heliRotation = heliRollRotation * heliPitchRotation;

        // 2. 기관포 자체 회전 (헬리콥터 회전 이후 추가 회전)
        glm::mat4 cannonYawRotation = glm::rotate(glm::mat4(1.0f), targetCameraXAngle, targetUp);
        glm::mat4 cannonPitchRotation = glm::rotate(glm::mat4(1.0f), targetCameraYAngle, right);

        // 최종 회전 = 헬리콥터 회전 + 기관포 회전
        glm::mat4 totalRotation = heliRotation * cannonYawRotation * cannonPitchRotation;

        // 3. 카메라 위치 계산 (기관포 위치 기준)
        glm::vec3 localOffset = -targetForward * gunnerOffset.x
            + targetUp * gunnerOffset.y
            + right * gunnerOffset.z;

        glm::vec4 rotatedOffset = totalRotation * glm::vec4(localOffset, 0.0f);
        position = cannonWorldPos + glm::vec3(rotatedOffset);

        // 4. 시선 방향 계산
        glm::vec3 baseForward = -targetForward;
        glm::vec4 finalForward = totalRotation * glm::vec4(baseForward, 0.0f);
        glm::vec3 lookDirection = glm::normalize(glm::vec3(finalForward));

        // 카메라 타겟 = 현재 위치 + 시선 방향 * 거리
        target = position + lookDirection * 100.0f;

        // 5. Up 벡터 계산
        glm::vec4 rotatedUp = totalRotation * glm::vec4(targetUp, 0.0f);
        up = glm::normalize(glm::vec3(rotatedUp));

        break;
    }
    }
}

void Camera::ProcessMouseDrag(int deltaX, int deltaY, float& heliYawRotation, float& heliCannonYaw, float& heliCannonPitch)
{

    if (cameraMode == 0 || cameraMode == 1)
    {
        heliYawRotation += deltaX * rotationSpeed * 100.0f;

        targetCameraXAngle -= deltaX * rotationSpeed;

        targetCameraYAngle += deltaY * rotationSpeed;

        const float maxAngle = glm::radians(89.0f);
        if (targetCameraYAngle > maxAngle) targetCameraYAngle = maxAngle;
        if (targetCameraYAngle < -maxAngle) targetCameraYAngle = -maxAngle;
    }
    else // 기관포 뷰 (cameraMode == 2)
    {
        // 마우스 움직임을 기관포의 Yaw/Pitch로 변환
        float sensitivity = rotationSpeed * 100.0f;
        
        // Yaw: 좌우 회전 (헬리콥터 기준 Y축)
        heliCannonYaw -= deltaX * sensitivity;
        
        // Pitch: 상하 회전 (헬리콥터 기준 Z축)
        heliCannonPitch -= deltaY * sensitivity;
        
        float maxPitch = 0.0f;
		if (heliCannonPitch > maxPitch) heliCannonPitch = maxPitch;

        targetCameraXAngle = glm::radians(heliCannonYaw);
        targetCameraYAngle = glm::radians(heliCannonPitch);
    }
  
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