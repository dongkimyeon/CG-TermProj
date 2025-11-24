#include "Helicopter.h"
#include "Input.h"
#include "Time.h"
#include "AA.h"

Helicopter::Helicopter()
	: position(glm::vec3(0.0f, 180.0f,0.0f))
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
	, tiltSpeed(8.0f)                  
	, mainBladeRotation(0.0f)
	, mainBladeSpeed(2500.0f)           
	, tailBladeRotation(0.0f)
	, tailBladeSpeed(2500.0f)
	, gravity(9.81f)                 
	, maxSpeed(101.4f)                 
	, accelerationRate(25.0f)          
	, drag(1.2f)                      
	, maxTiltAngle(35.0f)               
	, liftForce(0.0f)
	, maxLiftForce(60.0f)               
	, debugRotationX(0.0f)
	, debugRotationY(0.0f)
	, debugRotationZ(0.0f)
	, vaoBody(0), vboBody(0), eboBody(0)
	, vaoBlade(0), vboBlade(0), eboBlade(0)
	, vaoTail(0), vboTail(0), eboTail(0)
	, vaoCannon(0), vboCannon(0), eboCannon(0)
	, cannonOffset(28.0f, -16.5f, 0.0f)
	, cannonPitch(0.0f)
	, cannonYaw(0.0f)
{
}

Helicopter::~Helicopter()
{
	// 미사일 메모리 해제
	for (auto* missile : missiles) {
		delete missile;
	}
	for (auto* missile : attachedMissiles) {
		delete missile;
	}
	
	// 버퍼 정리
	CleanupBuffers();
}

void Helicopter::InitBuffers()
{
	const GLsizei stride = 11 * sizeof(GLfloat);
	
	// Body 버퍼
	glGenVertexArrays(1, &vaoBody);
	glBindVertexArray(vaoBody);

	glGenBuffers(1, &vboBody);
	glBindBuffer(GL_ARRAY_BUFFER, vboBody);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &eboBody);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBody);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);

	// Blade 버퍼
	glGenVertexArrays(1, &vaoBlade);
	glBindVertexArray(vaoBlade);

	glGenBuffers(1, &vboBlade);
	glBindBuffer(GL_ARRAY_BUFFER, vboBlade);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &eboBlade);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBlade);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);

	// Tail 버퍼
	glGenVertexArrays(1, &vaoTail);
	glBindVertexArray(vaoTail);

	glGenBuffers(1, &vboTail);
	glBindBuffer(GL_ARRAY_BUFFER, vboTail);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &eboTail);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboTail);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);

	// Cannon 버퍼
	glGenVertexArrays(1, &vaoCannon);
	glBindVertexArray(vaoCannon);

	glGenBuffers(1, &vboCannon);
	glBindBuffer(GL_ARRAY_BUFFER, vboCannon);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &eboCannon);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboCannon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);
}

void Helicopter::CleanupBuffers()
{
	// Body 버퍼 삭제
	if (vboBody != 0) glDeleteBuffers(1, &vboBody);
	if (eboBody != 0) glDeleteBuffers(1, &eboBody);
	if (vaoBody != 0) glDeleteVertexArrays(1, &vaoBody);
	
	// Blade 버퍼 삭제
	if (vboBlade != 0) glDeleteBuffers(1, &vboBlade);
	if (eboBlade != 0) glDeleteBuffers(1, &eboBlade);
	if (vaoBlade != 0) glDeleteVertexArrays(1, &vaoBlade);
	
	// Tail 버퍼 삭제
	if (vboTail != 0) glDeleteBuffers(1, &vboTail);
	if (eboTail != 0) glDeleteBuffers(1, &eboTail);
	if (vaoTail != 0) glDeleteVertexArrays(1, &vaoTail);
	
	// Cannon 버퍼 삭제
	if (vboCannon != 0) glDeleteBuffers(1, &vboCannon);
	if (eboCannon != 0) glDeleteBuffers(1, &eboCannon);
	if (vaoCannon != 0) glDeleteVertexArrays(1, &vaoCannon);
}

void Helicopter::Initialize()
{
	position = glm::vec3(0.0f, 180.0f,0.0f);
	velocity = glm::vec3(0.0f);
	acceleration = glm::vec3(0.0f);
	yRotation = 0.0f;

	// 버퍼 초기화
	InitBuffers();

	for (int i = 0; i < maxMissiles; ++i) {
		Missile* missile = new Missile();
		missile->Initialize();
		attachedMissiles.push_back(missile);
	}

	UpdateMissilePositions();
}

void Helicopter::LoadModels()
{
	// FBX 로드 - Helicopter 폴더의 Models 하위 폴더에서 로드
	if (!LoadFBX("Helicopter/Models/HeliBlade.FBX", &bladeModel)) {
		std::cerr << "HeliBlade FBX 로드 실패." << std::endl;
	}
	else {
		bladeModel.textureList.resize(1);
		bladeModel.textureList[0] = new Texture("Helicopter/Textures/HeliTexture.png");
		bladeModel.textureList[0]->LoadTexture();
		bladeModel.normalMap = new Texture("Helicopter/Textures/T_West_Heli_AH64D_N.png");
		bladeModel.normalMap->LoadTexture();
		UpdateModelBuffers(&bladeModel, vaoBlade, vboBlade, eboBlade);
		std::cout << "HeliBlade 로드 성공: " << bladeModel.vertices.size() / 11 << " vertices" << std::endl;
	}

	if (!LoadFBX("Helicopter/Models/HeliBody.FBX", &bodyModel)) {
		std::cerr << "HeliBody FBX 로드 실패." << std::endl;
	}
	else {
		bodyModel.textureList.resize(2);
		bodyModel.textureList[0] = new Texture("Helicopter/Textures/HeliTexture.png");
		bodyModel.textureList[0]->LoadTexture();
		bodyModel.textureList[1] = new Texture("Helicopter/Textures/GlassTexture.png");
		bodyModel.textureList[1]->LoadTexture();
		bodyModel.normalMap = new Texture("Helicopter/Textures/T_West_Heli_AH64D_N.png");
		bodyModel.normalMap->LoadTexture();
		UpdateModelBuffers(&bodyModel, vaoBody, vboBody, eboBody);

		std::cout << "HeliBody 로드 성공: " << bodyModel.vertices.size() / 11 << " vertices" << std::endl;
	}

	if (!LoadFBX("Helicopter/Models/HeliTail.FBX", &tailModel)) {
		std::cerr << "HeliTail FBX 로드 실패." << std::endl;
	}
	else { 
		tailModel.textureList.resize(1);
		tailModel.textureList[0] = new Texture("Helicopter/Textures/HeliTexture.png");
		tailModel.textureList[0]->LoadTexture();
		tailModel.normalMap = new Texture("Helicopter/Textures/T_West_Heli_AH64D_N.png");
		tailModel.normalMap->LoadTexture();
		UpdateModelBuffers(&tailModel, vaoTail, vboTail, eboTail);
		std::cout << "HeliTail 로드 성공: " << tailModel.vertices.size() / 11 << " vertices" << std::endl;
	}

	if (!LoadFBX("Helicopter/Models/HeliCannon.FBX", &CannonModel)) {
		std::cerr << "HeliCannon FBX 로드 실패." << std::endl;
	}
	else {
		CannonModel.textureList.resize(1);
		CannonModel.textureList[0] = new Texture("Helicopter/Textures/HeliTexture.png");
		CannonModel.textureList[0]->LoadTexture();
		CannonModel.normalMap = new Texture("Helicopter/Textures/T_West_Heli_AH64D_N.png");
		CannonModel.normalMap->LoadTexture();
		UpdateModelBuffers(&CannonModel, vaoCannon, vboCannon, eboCannon);
		std::cout << "HeliCannon 로드 성공: " << CannonModel.vertices.size() / 11 << " vertices" << std::endl;
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

	// 미사일 업데이트
	UpdateMissiles(deltaTime);
	UpdateMissilePositions();
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

	// 미사일 발사 (F 키)
	if (Input::GetKeyDown(eKeyCode::F))
	{
		FireMissile();
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

glm::mat4 Helicopter::GetHelicopterTransform() const
{
	glm::mat4 worldModelMat = glm::mat4(1.0f);
	worldModelMat = glm::translate(worldModelMat, position);
	worldModelMat = glm::rotate(worldModelMat, glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));

	// 물리 기반 기울기 적용
	worldModelMat = glm::rotate(worldModelMat, glm::radians(currentRoll), glm::vec3(1.0f, 0.0f, 0.0f));
	worldModelMat = glm::rotate(worldModelMat, glm::radians(currentPitch), glm::vec3(0.0f, 0.0f, 1.0f));

	// 디버그 회전
	worldModelMat = glm::rotate(worldModelMat, glm::radians(debugRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	worldModelMat = glm::rotate(worldModelMat, glm::radians(debugRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	worldModelMat = glm::rotate(worldModelMat, glm::radians(debugRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

	return worldModelMat;
}

glm::vec3 Helicopter::GetMissileAttachmentPosition() const
{
	glm::mat4 heliTransform = GetHelicopterTransform();
	glm::vec3 attachmentOffset = glm::vec3(0.0f, missileAttachmentOffset, 0.0f);
	glm::vec4 worldAttachmentPos = heliTransform * glm::vec4(attachmentOffset, 1.0f);
	return glm::vec3(worldAttachmentPos);
}

glm::vec3 Helicopter::GetCannonWorldPosition() const
{
	// 헬리콥터의 월드 변환 매트릭스
	glm::mat4 heliTransform = GetHelicopterTransform();
	
	// 기관포 힌지의 월드 위치 계산
	// cannonOffset + cannonHingePos = 기관포 힌지의 로컬 위치
	glm::vec3 hingeLocalPos = cannonOffset + cannonHingePos;
	glm::vec4 hingeWorldPos = heliTransform * glm::vec4(hingeLocalPos, 1.0f);
	
	return glm::vec3(hingeWorldPos);
}

void Helicopter::UpdateMissilePositions()
{
	if (attachedMissiles.empty()) return;

	glm::mat4 heliTransform = GetHelicopterTransform();

	// 미사일들을 헬리콥터 아래쪽에 일렬로 배치
	float totalWidth = (attachedMissiles.size() - 1) * missileSpacing;
	float startOffset = -totalWidth * 0.5f;

	for (size_t i = 0; i < attachedMissiles.size(); ++i) {
		float xOffset = startOffset + i * missileSpacing;
		glm::vec3 localOffset = glm::vec3(xOffset, missileAttachmentOffset, 0.0f);
		glm::vec4 worldPos = heliTransform * glm::vec4(localOffset, 1.0f);

		attachedMissiles[i]->SetPosition(glm::vec3(worldPos));

		// 헬리콥터의 기저벡터 forward 방향에 피치를 반영
		float pitchRad = glm::radians(currentPitch);

		// forward 벡터를 기준으로 피치 적용
		glm::vec3 baseForward = glm::normalize(forward); // 기저벡터의 forward 사용
		glm::vec3 baseUp = glm::normalize(up);      // 기저벡터의 up 사용

		// 피치 회전을 위한 축 (right 벡터)
		glm::vec3 rightAxis = glm::normalize(glm::cross(baseForward, baseUp));

		// 피치 회전 매트릭스 생성
		glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), pitchRad, rightAxis);

		// forward 벡터에 피치 적용
		glm::vec4 pitchedForward = pitchRotation * glm::vec4(baseForward, 0.0f);
		glm::vec3 missileDirection = glm::normalize(glm::vec3(pitchedForward));

		attachedMissiles[i]->SetDirection(missileDirection);
	}
}

void Helicopter::FireMissile()
{
	if (attachedMissiles.empty()) return;

	// 첫 번째 미사일을 발사
	Missile* missileToFire = attachedMissiles.front();
	attachedMissiles.erase(attachedMissiles.begin());

	// 발사 위치: 헬리콥터 전방 약간 앞쪽에서 발사
	glm::mat4 heliTransform = GetHelicopterTransform();
	glm::vec3 forwardOffset = glm::vec3(5.0f, 0.0f, 0.0f); // 헬리콥터 전방으로 5 단위
	glm::vec4 launchPosWorld = heliTransform * glm::vec4(forwardOffset, 1.0f);
	glm::vec3 launchPos = glm::vec3(launchPosWorld);

	// 발사 방향: 헬리콥터의 기저벡터 forward 방향에 피치 반영
	float pitchRad = - glm::radians(currentPitch);

	// forward 벡터를 기준으로 피치 적용
	glm::vec3 baseForward = glm::normalize(forward); // 기저벡터의 forward 사용
	glm::vec3 baseUp = glm::normalize(up);   // 기저벡터의 up 사용

	// 피치 회전을 위한 축 (right 벡터)
	glm::vec3 rightAxis = glm::normalize(glm::cross(baseForward, baseUp));

	// 피치 회전 매트릭스 생성
	glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), pitchRad, rightAxis);

	// forward 벡터에 피치 적용
	glm::vec4 pitchedForward = pitchRotation * glm::vec4(baseForward, 0.0f);
	glm::vec3 launchDir = -glm::normalize(glm::vec3(pitchedForward));

	missileToFire->Launch(launchPos, launchDir);
	missiles.push_back(missileToFire);

	std::cout << "미사일 발사! 남은 미사일: " << attachedMissiles.size() << std::endl;
}

void Helicopter::UpdateMissiles(float deltaTime)
{
	// 발사된 미사일들 업데이트
	for (auto it = missiles.begin(); it != missiles.end();) {
		(*it)->Update(deltaTime);

		// 비활성화된 미사일 제거
		if (!(*it)->IsActive()) {
			delete* it;
			it = missiles.erase(it);
		}
		else {
			++it;
		}
	}
}

void Helicopter::RenderMissiles(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj)
{
	// 부착된 미사일들 렌더링
	for (auto* missile : attachedMissiles) {
		missile->Render(shaderID, view, proj);
	}

	// 발사된 미사일들 렌더링
	for (auto* missile : missiles) {
		missile->Render(shaderID, view, proj);
	}
}



void Helicopter::Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale)
{
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	GLint textureLoc = glGetUniformLocation(shaderID, "textureSampler");
	GLint normalMapLoc = glGetUniformLocation(shaderID, "normalMap");
	GLint alphaValueLoc = glGetUniformLocation(shaderID, "alphaValue");
	GLint useNormalMapLoc = glGetUniformLocation(shaderID, "useNormalMap");
	GLint useTextureLoc = glGetUniformLocation(shaderID, "useTexture");

	// 헬리콥터는 텍스처를 사용
	glUniform1f(useTextureLoc, 1.0f);

	// 헬기 전체 모델 매트릭스
	glm::mat4 worldModelMat = GetHelicopterTransform();

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

	// 기관포 렌더링
	if (CannonModel.loaded && !CannonModel.indices.empty())
	{
		glUniform1f(alphaValueLoc, 1.0f);
		glUniform1i(useNormalMapLoc, CannonModel.normalMap != nullptr ? 1 : 0);

		glm::mat4 modelMat = worldModelMat;
		modelMat = glm::translate(modelMat, glm::vec3(cannonOffset));
		
		modelMat = glm::translate(modelMat, cannonHingePos);
		modelMat = glm::rotate(modelMat, glm::radians(cannonYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(cannonPitch), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMat = glm::translate(modelMat, -cannonHingePos);
		modelMat = glm::scale(modelMat, glm::vec3(modelScale));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

		glBindVertexArray(vaoCannon);
		if (wireframeMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		for (size_t i = 0; i < CannonModel.meshes.size(); ++i) {
			const auto& meshInfo = CannonModel.meshes[i];

			if (meshInfo.materialIndex < CannonModel.textureList.size() &&
				CannonModel.textureList[meshInfo.materialIndex]) {
				CannonModel.textureList[meshInfo.materialIndex]->UseTexture(0);
				glUniform1i(textureLoc, 0);
			}

			if (CannonModel.normalMap) {
				CannonModel.normalMap->UseTexture(1);
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
