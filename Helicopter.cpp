#include "Helicopter.h"
#include "Input.h"
#include "Time.h"

Helicopter::Helicopter()
    : position(glm::vec3(0.0f))
    , velocity(glm::vec3(0.0f))
    , acceleration(glm::vec3(0.0f))
    , forward(glm::vec3(0.0f, 0.0f, 1.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , right(glm::vec3(1.0f, 0.0f, 0.0f))
    , yRotation(0.0f)
    , targetPitch(0.0f)
    , targetRoll(0.0f)
    , currentPitch(0.0f)
    , currentRoll(0.0f)
    , tiltSpeed(5.0f)
    , mainBladeRotation(0.0f)
    , mainBladeSpeed(2000.0f)
    , tailBladeRotation(0.0f)
    , tailBladeSpeed(2000.0f)
    , gravity(9.81f)
    , maxSpeed(50.0f)
    , accelerationRate(30.0f)
    , drag(2.0f)
    , maxTiltAngle(30.0f)
    , liftForce(0.0f)
    , maxLiftForce(50.0f)
    , debugRotationX(0.0f)
    , debugRotationY(0.0f)
    , debugRotationZ(0.0f)
    , vaoBody(0), vboBody(0), eboBody(0)
    , vaoBlade(0), vboBlade(0), eboBlade(0)
    , vaoTail(0), vboTail(0), eboTail(0)
{
}

Helicopter::~Helicopter()
{
}

void Helicopter::Initialize()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    yRotation = 0.0f;
}

void Helicopter::LoadModels(GLuint vaoB, GLuint vboB, GLuint eboB,
                            GLuint vaoBl, GLuint vboBl, GLuint eboBl,
                            GLuint vaoT, GLuint vboT, GLuint eboT)
{
    vaoBody = vaoB;
    vboBody = vboB;
    eboBody = eboB;
    
    vaoBlade = vaoBl;
    vboBlade = vboBl;
    eboBlade = eboBl;
    
    vaoTail = vaoT;
    vboTail = vboT;
    eboTail = eboT;
    
    // FBX 로드
    if (!LoadFBX("HeliBlade.FBX", &bladeModel)) {
        std::cerr << "HeliBlade FBX 로드 실패." << std::endl;
    }
    else {
        UpdateModelBuffers(&bladeModel, vaoBlade, vboBlade, eboBlade);
    }
    
    if (!LoadFBX("HeliBody.FBX", &bodyModel)) {
        std::cerr << "HeliBody FBX 로드 실패." << std::endl;
    }
    else {
        UpdateModelBuffers(&bodyModel, vaoBody, vboBody, eboBody);
    }
    
    if (!LoadFBX("HeliTail.FBX", &tailModel)) {
        std::cerr << "HeliTail FBX 로드 실패." << std::endl;
    }
    else {
        UpdateModelBuffers(&tailModel, vaoTail, vboTail, eboTail);
    }
}

void Helicopter::Update(float deltaTime)
{
    // 블레이드 회전
    mainBladeRotation += mainBladeSpeed * deltaTime;
    tailBladeRotation += tailBladeSpeed * deltaTime;
    
    // 입력 처리
    ProcessInput(deltaTime);
    
    // 물리 업데이트
    UpdatePhysics(deltaTime);
    
    // 방향 업데이트
    UpdateOrientation(deltaTime);
}

void Helicopter::ProcessInput(float deltaTime)
{
    // 현재 헬기의 회전 방향 벡터 계산
    float yawRad = glm::radians(yRotation);
    glm::vec3 forwardDir = glm::vec3(cos(yawRad), 0.0f, -sin(yawRad));
    glm::vec3 rightDir = glm::vec3(sin(yawRad), 0.0f, cos(yawRad));
    
    // 전진
    if (Input::GetKey(eKeyCode::W))
    {
        acceleration += forwardDir * accelerationRate;
        targetPitch = -maxTiltAngle;
    }
    // 후진
    if (Input::GetKey(eKeyCode::S))
    {
        acceleration -= forwardDir * accelerationRate;
        targetPitch = maxTiltAngle;
    }
    
    // 좌측 이동
    if (Input::GetKey(eKeyCode::A))
    {
        acceleration -= rightDir * accelerationRate;
        targetRoll = -maxTiltAngle;
    }
    // 우측 이동
    if (Input::GetKey(eKeyCode::D))
    {
        acceleration += rightDir * accelerationRate;
        targetRoll = maxTiltAngle;
    }
    
    // 기울기 복원
    if (!Input::GetKey(eKeyCode::W) && !Input::GetKey(eKeyCode::S))
    { 
        targetPitch = 0.0f;
    }
    if (!Input::GetKey(eKeyCode::A) && !Input::GetKey(eKeyCode::D))
    {
        targetRoll = 0.0f;
    }
    
    // 고도 제어
    if (Input::GetKey(eKeyCode::SPACE))
    {
        liftForce = maxLiftForce;
    }
    else if (Input::GetKey(eKeyCode::SHIFT))
    {
        liftForce = -maxLiftForce * 0.5f;
    }
    else
    {
        liftForce = gravity; // 호버링
    }
}

void Helicopter::UpdatePhysics(float deltaTime)
{
    // 1. 공기 저항 적용
    glm::vec3 dragForce = -velocity * drag;
    acceleration += dragForce * deltaTime;
    
    // 2. 속도 업데이트
    velocity += acceleration * deltaTime;
    
    // 3. 최대 속도 제한
    float currentSpeed = glm::length(velocity);
    if (currentSpeed > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    
    // 4. 위치 업데이트
    position += velocity * deltaTime;
    
    // 5. 중력과 양력 적용
    float netVerticalForce = liftForce - gravity;
    position.y += netVerticalForce * deltaTime;
    
    // 지면 충돌 방지
    if (position.y < 0.0f) {
        position.y = 0.0f;
        velocity.y = 0.0f;
    }
    
    // 6. 기울기 부드럽게 보간
    currentPitch = glm::mix(currentPitch, targetPitch, tiltSpeed * deltaTime);
    currentRoll = glm::mix(currentRoll, targetRoll, tiltSpeed * deltaTime);
    
    // 7. 가속도 초기화
    acceleration = glm::vec3(0.0f);
}

void Helicopter::UpdateOrientation(float deltaTime)
{
    // 카메라용 회전 매트릭스: Y축 회전(yaw)만 적용
    glm::mat4 rotationMat = glm::mat4(1.0f);
    rotationMat = glm::rotate(rotationMat, glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // 헬기의 기본 방향 벡터
    glm::vec3 baseForward = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 baseUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    // 기저벡터 업데이트 (Y축 회전만 반영)
    forward = glm::vec3(rotationMat * glm::vec4(baseForward, 0.0f));
    up = glm::vec3(rotationMat * glm::vec4(baseUp, 0.0f));
    right = glm::cross(forward, up);
}

void Helicopter::Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale)
{
    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    GLint textureLoc = glGetUniformLocation(shaderID, "textureSampler");
    GLint normalMapLoc = glGetUniformLocation(shaderID, "normalMap");
    GLint alphaValueLoc = glGetUniformLocation(shaderID, "alphaValue");
    GLint useNormalMapLoc = glGetUniformLocation(shaderID, "useNormalMap");
    
    // 헬기 전체 모델 매트릭스
    glm::mat4 worldModelMat = glm::mat4(1.0f);
    worldModelMat = glm::translate(worldModelMat, position);
    worldModelMat = glm::rotate(worldModelMat, glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // 물리 기반 기울기 적용
    worldModelMat = glm::rotate(worldModelMat, glm::radians(currentRoll), glm::vec3(1.0f, 0.0f, 0.0f));
    worldModelMat = glm::rotate(worldModelMat, glm::radians(currentPitch), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // 디버그 회전
    worldModelMat = glm::rotate(worldModelMat, glm::radians(debugRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    worldModelMat = glm::rotate(worldModelMat, glm::radians(debugRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // 몸체 렌더링
    if (bodyModel.loaded && !bodyModel.indices.empty())
    {
        glUniform1i(useNormalMapLoc, bodyModel.normalMap != nullptr ? 1 : 0);
        
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
        
        glBindVertexArray(vaoBody);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        for (size_t i = 0; i < bodyModel.meshes.size(); ++i) {
            const auto& meshInfo = bodyModel.meshes[i];
            
            if (meshInfo.materialIndex == 1) {
                glUniform1f(alphaValueLoc, glassAlpha);
                glDepthMask(GL_FALSE);
            }
            else {
                glUniform1f(alphaValueLoc, 1.0f);
                glDepthMask(GL_TRUE);
            }
            
            if (meshInfo.materialIndex < bodyModel.textureList.size() &&
                bodyModel.textureList[meshInfo.materialIndex]) {
                bodyModel.textureList[meshInfo.materialIndex]->UseTexture(0);
                glUniform1i(textureLoc, 0);
            }
            
            if (bodyModel.normalMap) {
                bodyModel.normalMap->UseTexture(1);
                glUniform1i(normalMapLoc, 1);
            }
            
            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT,
                (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }
        
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // 메인 블레이드 렌더링
    if (bladeModel.loaded && !bladeModel.indices.empty())
    {
        glUniform1f(alphaValueLoc, 1.0f);
        glUniform1i(useNormalMapLoc, bladeModel.normalMap != nullptr ? 1 : 0);
        
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::translate(modelMat, glm::vec3(2.5f, 18.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(mainBladeRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
        
        glBindVertexArray(vaoBlade);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        for (size_t i = 0; i < bladeModel.meshes.size(); ++i) {
            const auto& meshInfo = bladeModel.meshes[i];
            
            if (meshInfo.materialIndex < bladeModel.textureList.size() &&
                bladeModel.textureList[meshInfo.materialIndex]) {
                bladeModel.textureList[meshInfo.materialIndex]->UseTexture(0);
                glUniform1i(textureLoc, 0);
            }
            
            if (bladeModel.normalMap) {
                bladeModel.normalMap->UseTexture(1);
                glUniform1i(normalMapLoc, 1);
            }
            
            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT,
                (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }
        
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // 테일 블레이드 렌더링
    if (tailModel.loaded && !tailModel.indices.empty())
    {
        glUniform1f(alphaValueLoc, 1.0f);
        glUniform1i(useNormalMapLoc, tailModel.normalMap != nullptr ? 1 : 0);
        
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::translate(modelMat, glm::vec3(-88.0f, 17.0f, -7.0f));
        modelMat = glm::rotate(modelMat, glm::radians(tailBladeRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
        
        glBindVertexArray(vaoTail);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        for (size_t i = 0; i < tailModel.meshes.size(); ++i) {
            const auto& meshInfo = tailModel.meshes[i];
            
            if (meshInfo.materialIndex < tailModel.textureList.size() &&
                tailModel.textureList[meshInfo.materialIndex]) {
                tailModel.textureList[meshInfo.materialIndex]->UseTexture(0);
                glUniform1i(textureLoc, 0);
            }
            
            if (tailModel.normalMap) {
                tailModel.normalMap->UseTexture(1);
                glUniform1i(normalMapLoc, 1);
            }
            
            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT,
                (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }
        
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
