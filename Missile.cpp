#include "Missile.h"

Missile::Missile()
	: VAO(0), VBO(0), EBO(0), lightVAO(0), lightVBO(0), lightEBO(0),
	  position(0.0f), direction(0.0f, 0.0f, 1.0f), velocity(0.0f), 
	  isActive(false), smokeTrail(300), particleEmissionTimer(0.0f)
{
}

Missile::~Missile()
{
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
	if (VBO != 0) glDeleteBuffers(1, &VBO);
	if (EBO != 0) glDeleteBuffers(1, &EBO);
	if (lightVAO != 0) glDeleteVertexArrays(1, &lightVAO);
	if (lightVBO != 0) glDeleteBuffers(1, &lightVBO);
	if (lightEBO != 0) glDeleteBuffers(1, &lightEBO);
}

void Missile::Initialize()
{
	CreateCubeGeometry();
	SetupBuffers();
	CreateLightGeometry();
	SetupLightBuffers();
	
	
	smokeTrail = ParticleSystem(300);
	smokeTrail.initialize();
}

void Missile::CreateCubeGeometry()
{
	// 육면체의 반 크기
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	float halfDepth = depth * 0.5f;

	// 큰 배열을 힙으로 이동하여 스택 사용량 줄이기
	static const std::vector<GLfloat> cubeVerticesData = {
		// 앞면 (Z+)
		-halfWidth, -halfHeight,  halfDepth,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 0
		 halfWidth, -halfHeight,  halfDepth,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 1
		 halfWidth,  halfHeight,  halfDepth,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 2
		-halfWidth,  halfHeight,  halfDepth,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 3

		// 뒷면 (Z-)
		-halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 4
		-halfWidth,  halfHeight, -halfDepth, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 5
		 halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 6
		 halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 7

		// 왼쪽면 (X-)
		-halfWidth, -halfHeight, -halfDepth,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 8
		-halfWidth, -halfHeight,  halfDepth,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 9
		-halfWidth,  halfHeight,  halfDepth,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 10
		-halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 11

		// 오른쪽면 (X+)
		 halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 12
		 halfWidth,  halfHeight, -halfDepth,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 13
		 halfWidth,  halfHeight,  halfDepth,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 14
		 halfWidth, -halfHeight,  halfDepth,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 15

		// 윗면 (Y+)
		-halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 16
		-halfWidth,  halfHeight,  halfDepth,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 17
		 halfWidth,  halfHeight,  halfDepth,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 18
		 halfWidth,  halfHeight, -halfDepth,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 19

		// 아랫면 (Y-)
		-halfWidth, -halfHeight, -halfDepth,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 20
		 halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 21
		 halfWidth, -halfHeight,  halfDepth,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 22
		-halfWidth, -halfHeight,  halfDepth,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f  // 23
	};

	vertices = cubeVerticesData;

	// 육면체의 인덱스 (각 면은 2개의 삼각형으로 구성)
	static const std::vector<GLuint> cubeIndicesData = {
		// 앞면
		0, 1, 2,   2, 3, 0,
		// 뒷면
		4, 5, 6,   6, 7, 4,
		// 왼쪽면
		8, 9, 10,  10, 11, 8,
		// 오른쪽면
		12, 13, 14, 14, 15, 12,
		// 윗면
		16, 17, 18, 18, 19, 16,
		// 아랫면
		20, 21, 22, 22, 23, 20
	};

	indices = cubeIndicesData;
}

void Missile::CreateLightGeometry()
{
	// 조명을 위한 구체 생성
	float radius = 6.0f * 0.5f; // lightSize가 6.0f 였으니 반지름은 3.0f
	int segments = 24; // 구체의 부드러움을 결정 (높을수록 부드러움)

	lightVertices.clear();
	lightIndices.clear();

	// UV Sphere 생성 로직
	for (int y = 0; y <= segments; ++y) {
		float y_ratio = (float)y / (float)segments;
		float pitch = y_ratio * glm::pi<float>();
		float y_coord = radius * glm::cos(pitch);
		float slice_radius = radius * glm::sin(pitch);

		for (int x = 0; x <= segments; ++x) {
			float x_ratio = (float)x / (float)segments;
			float yaw = x_ratio * 2.0f * glm::pi<float>();

			float x_coord = slice_radius * glm::cos(yaw);
			float z_coord = slice_radius * glm::sin(yaw);

			glm::vec3 position = glm::vec3(x_coord, y_coord, z_coord);
			glm::vec3 normal = glm::normalize(position);
			glm::vec2 uv = glm::vec2(x_ratio, y_ratio);

			// 정점 데이터: 위치(3), UV(2), 법선(3), 탄젠트(3) - 총 11개
			lightVertices.push_back(position.x);
			lightVertices.push_back(position.y);
			lightVertices.push_back(position.z);

			lightVertices.push_back(uv.x);
			lightVertices.push_back(uv.y);

			lightVertices.push_back(normal.x);
			lightVertices.push_back(normal.y);
			lightVertices.push_back(normal.z);

			// 탄젠트는 복잡하니 임시로 (1, 0, 0) 설정.
			// 조명 쉐이더는 보통 법선/탄젠트를 사용하지 않으므로 큰 문제는 안됨.
			lightVertices.push_back(1.0f);
			lightVertices.push_back(0.0f);
			lightVertices.push_back(0.0f);
		}
	}

	// 인덱스 생성
	for (int y = 0; y < segments; ++y) {
		for (int x = 0; x < segments; ++x) {
			int current_row = y * (segments + 1);
			int next_row = (y + 1) * (segments + 1);

			int p1 = current_row + x;
			int p2 = current_row + x + 1;
			int p3 = next_row + x + 1;
			int p4 = next_row + x;

			// 두 개의 삼각형 (Quad)
			if (y != 0) { // 위쪽 극점 제외
				lightIndices.push_back(p1);
				lightIndices.push_back(p4);
				lightIndices.push_back(p2);
			}

			if (y != segments - 1) { // 아래쪽 극점 제외
				lightIndices.push_back(p2);
				lightIndices.push_back(p4);
				lightIndices.push_back(p3);
			}
		}
	}
}

void Missile::SetupBuffers()
{
	// VAO/VBO/EBO 생성
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// VBO 설정
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// EBO 설정
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	const GLsizei stride = 11 * sizeof(GLfloat);

	// 위치 (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// UV 좌표 (location 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// 법선 (location 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// 탄젠트 (location 3)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	
	// 파티클용 색상 속성 (location 4) - 기본값으로 설정 (미사일은 파티클이 아님)
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);

	std::cout << "Missile 초기화 완료 - 크기: " << width << " x " << height << " x " << depth
		<< ", 정점: " << vertices.size() / 11 << ", 인덱스: " << indices.size() << std::endl;
}

void Missile::SetupLightBuffers()
{
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glGenBuffers(1, &lightEBO);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, lightVertices.size() * sizeof(GLfloat), lightVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightIndices.size() * sizeof(GLuint), lightIndices.data(), GL_STATIC_DRAW);

	const GLsizei stride = 11 * sizeof(GLfloat);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	
	// 파티클용 색상 속성 (location 4) - 기본값으로 설정
	glVertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(0);
}

glm::vec3 Missile::GetLightPosition() const
{
	return position + direction * lightOffset;
}

void Missile::Update(float deltaTime)
{
	if (isActive)
	{
		// 현재 위치를 이전 위치로 저장
		glm::vec3 previousPosition = position;
		
		// 미사일 이동 - 중력 영향 없이 직선 이동
		velocity = direction * speed;
		position += velocity * deltaTime;

		// 펄스 효과 제거 - 일정한 조명 밝기 유지
		lightIntensity = 4.0f; // 고정된 밝기

		// 연기 파티클 생성 - 경로를 따라 생성
		particleEmissionTimer += deltaTime;
		float emissionInterval = 0.01f; // 간격 늘림 (더 균등한 선형 궤적)
		
		while (particleEmissionTimer >= emissionInterval)
		{
			glm::vec3 movementVector = position - previousPosition;
			float movementDistance = glm::length(movementVector);
			glm::vec3 moveDir = movementDistance > 0.0f ? movementVector / movementDistance : direction;
			
			// 이동 거리 기준으로 일정 간격으로 파티클 배치
			float spacing = trailSpacing; // 헤더에 정의된 선형 간격
			int numTrailPoints = (movementDistance > 0.0f) ? std::max(1, (int)(movementDistance / spacing) + 1) : 1;
			
			for (int i = 0; i < numTrailPoints; ++i)
			{
				float t = (numTrailPoints > 1) ? float(i) / float(numTrailPoints - 1) : 0.0f;
				glm::vec3 interpolatedPos = glm::mix(previousPosition, position, t);
				// 미사일 뒤쪽(방향 반대로 height 일부)으로 오프셋
				glm::vec3 smokePosition = interpolatedPos - direction * (height * 0.6f);
				// 선형 궤적: 난수 오프셋 제거, 약간 위로 뜨는 느낌만 추가
				glm::vec3 smokeVelocity = glm::vec3(0.0f, 5.0f, 0.0f); // 약간 상승
				smokeTrail.emitParticle(smokePosition, smokeVelocity);
			}
			particleEmissionTimer -= emissionInterval;
		}
		
		// 파티클 시스템 업데이트
		smokeTrail.update(deltaTime);

		// 땅에 닿거나 범위를 벗어나면 비활성화
		if (position.y <= 0.0f || glm::length(position) > 2000.0f)
		{
			isActive = false;
		}
	}
	else
	{
		// 미사일이 비활성화되어도 파티클은 계속 업데이트
		smokeTrail.update(deltaTime);
	}
}

void Missile::Render(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj)
{
	// 미사일 렌더링
	if (isActive)
	{
		glUseProgram(shaderProgramID);

		// 유니폼 위치 가져오기
		GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
		GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
		GLint colorLoc = glGetUniformLocation(shaderProgramID, "aColor");
		GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
		GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");

		// 모델 행렬 생성 (위치와 방향에 따른 변환)
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);

		// 방향에 따른 회전 - 미사일의 긴 축(Y축)이 방향을 향하도록 설정
		if (glm::length(direction) > 0.0f)
		{
			glm::vec3 forward = glm::normalize(direction);
			glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

			if (abs(glm::dot(forward, worldUp)) > 0.99f) {
				worldUp = glm::vec3(1.0f, 0.0f, 0.0f);
			}

			glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
			glm::vec3 up = glm::cross(forward, right);

			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			rotationMatrix[0] = glm::vec4(right, 0.0f);
			rotationMatrix[1] = glm::vec4(forward, 0.0f);
			rotationMatrix[2] = glm::vec4(up, 0.0f);

			model *= rotationMatrix;
		}

		// 행렬 및 색상 설정
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3fv(colorLoc, 1, glm::value_ptr(missileColor));
		glUniform1f(useTextureLoc, 0.0f);
		glUniform1f(alphaValueLoc, 1.0f);

		// 미사일 렌더링 (경고 수정: size_t를 GLsizei로 안전하게 변환)
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// 조명 렌더링
		RenderMissileLight(shaderProgramID, view, proj);

		// 텍스처 사용 다시 활성화
		glUniform1f(useTextureLoc, 1.0f);
	}
	
	// smokeTrail.render may modify GL state; save/restore minimal state around it
	GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean prevBlend = glIsEnabled(GL_BLEND);
	GLboolean prevDepthMask = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);

	smokeTrail.render(view, proj);

	// restore
	glDepthMask(prevDepthMask ? GL_TRUE : GL_FALSE);
	if (!prevBlend) glDisable(GL_BLEND);
	else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (prevDepthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void Missile::RenderMissileLight(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj)
{
	if (!isActive) return;

	// Save minimal GL states
	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean blendEnabled = glIsEnabled(GL_BLEND);
	GLboolean depthWriteMask = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteMask);

	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
	GLint colorLoc = glGetUniformLocation(shaderProgramID, "aColor");
	GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
	GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");

	// Light transform
	glm::vec3 lightPos = GetLightPosition();
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(lightIntensity));

	glm::vec3 brightLightColor = glm::vec3(1.0f,0.8f,0.0f) * lightIntensity;

	// Set states for additive light rendering
	if (depthTestEnabled) glDisable(GL_DEPTH_TEST);
	if (!blendEnabled) glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive for glow
	glDepthMask(GL_FALSE);

	// Render light mesh
	glUseProgram(shaderProgramID);
	glUniformMatrix4fv(modelLoc,1, GL_FALSE, glm::value_ptr(lightModel));
	glUniformMatrix4fv(viewLoc,1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc,1, GL_FALSE, glm::value_ptr(proj));
	glUniform3fv(colorLoc,1, glm::value_ptr(brightLightColor));
	glUniform1f(useTextureLoc,0.0f);
	glUniform1f(alphaValueLoc,1.0f);

	glBindVertexArray(lightVAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(lightIndices.size()), GL_UNSIGNED_INT,0);
	glBindVertexArray(0);

	// Restore GL state
	glDepthMask(depthWriteMask ? GL_TRUE : GL_FALSE);
	if (!blendEnabled) glDisable(GL_BLEND);
	else {
		// restore to standard alpha blending used elsewhere
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
}

void Missile::Launch(const glm::vec3& startPos, const glm::vec3& dir)
{
	position = startPos;
	direction = glm::normalize(dir);
	velocity = direction * speed;
	isActive = true;
	particleEmissionTimer = 0.0f;
	smokeTrail.clear();
}

void Missile::SetPosition(const glm::vec3& pos)
{
	position = pos;
}

void Missile::SetDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
}

bool Missile::IsFinished() const
{
	// Missile is finished when it's inactive AND the particle system has no live particles
	return (!isActive) && (!smokeTrail.hasLiveParticles());
}
