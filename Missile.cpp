#include "Missile.h"

Missile::Missile()
	: VAO(0), VBO(0), EBO(0), lightVAO(0), lightVBO(0), lightEBO(0),
	  position(0.0f), direction(0.0f, 0.0f, 1.0f), velocity(0.0f), 
	  isActive(false), smokeTrail(500), particleEmissionTimer(0.0f), lightPulseTimer(0.0f)
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
		 halfWidth,  halfHeight,  halfDepth,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f, // 2
		-halfWidth,  halfHeight,  halfDepth,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 3

		// 뒷면 (Z-)
		-halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 4
		-halfWidth,  halfHeight, -halfDepth,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 5
		 halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 6
		 halfWidth, -halfHeight, -halfDepth,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 7

		// 왼쪽면 (X-)
		-halfWidth, -halfHeight, -halfDepth,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 8
		-halfWidth, -halfHeight,  halfDepth,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 9
		-halfWidth,  halfHeight,  halfDepth,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 10
		-halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 11

		// 오른쪽면 (X+)
		 halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 12
		 halfWidth,  halfHeight, -halfDepth,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 13
		 halfWidth,  halfHeight,  halfDepth,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 14
		 halfWidth, -halfHeight,  halfDepth,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,0.0f, 0.0f, -1.0f, // 15

		// 윗면 (Y+)
		-halfWidth,  halfHeight, -halfDepth,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 16
		-halfWidth,  halfHeight,  halfDepth,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 17
		 halfWidth,  halfHeight,  halfDepth,   1.0f, 0.0f,0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 18
		 halfWidth,  halfHeight, -halfDepth,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 19

		// 아랫면 (Y-)
		-halfWidth, -halfHeight, -halfDepth,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 20
		 halfWidth, -halfHeight, -halfDepth,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 21
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
	// 조명을 위한 작은 구체 (간단한 육면체로 대체)
	float lightSize = 2.0f;
	float halfSize = lightSize * 0.5f;

	// 큰 배열을 정적으로 선언하여 스택 사용량 줄이기
	static const std::vector<GLfloat> lightVerticesData = {
		// 앞면 (Z+)
		-halfSize, -halfSize,  halfSize,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 0
		 halfSize, -halfSize,  halfSize,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 1
		 halfSize,  halfSize,  halfSize,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // 2
		-halfSize,  halfSize,  halfSize,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 3

		// 뒷면 (Z-)
		-halfSize, -halfSize, -halfSize,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 4
		-halfSize,  halfSize, -halfSize,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 5
		 halfSize,  halfSize, -halfSize,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 6
		 halfSize, -halfSize, -halfSize,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f, // 7

		// 왼쪽면 (X-)
		-halfSize, -halfSize, -halfSize,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 8
		-halfSize, -halfSize,  halfSize,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 9
		-halfSize,  halfSize,  halfSize,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 10
		-halfSize,  halfSize, -halfSize,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 11

		// 오른쪽면 (X+)
		 halfSize, -halfSize, -halfSize,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 12
		 halfSize,  halfSize, -halfSize,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 13
		 halfSize,  halfSize,  halfSize,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 14
		 halfSize, -halfSize,  halfSize,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f, // 15

		// 윗면 (Y+)
		-halfSize,  halfSize, -halfSize,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 16
		-halfSize,  halfSize,  halfSize,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 17
		 halfSize,  halfSize,  halfSize,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 18
		 halfSize,  halfSize, -halfSize,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 19

		// 아랫면 (Y-)
		-halfSize, -halfSize, -halfSize,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 20
		 halfSize, -halfSize, -halfSize,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 21
		 halfSize, -halfSize,  halfSize,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 22
		-halfSize, -halfSize,  halfSize,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f  // 23
	};

	lightVertices = lightVerticesData;

	static const std::vector<GLuint> lightIndicesData = {
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

	lightIndices = lightIndicesData;
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
		// 미사일 이동 - 중력 영향 없이 직선 이동
		velocity = direction * speed;
		position += velocity * deltaTime;

		// 조명 펄스 효과
		lightPulseTimer += deltaTime * 8.0f; // 빠른 펄스
		lightIntensity = 2.0f + sin(lightPulseTimer) * 0.5f; // 1.5 ~ 2.5 범위

		// 연기 파티클 생성 - 더 자주 생성하도록 수정
		particleEmissionTimer += deltaTime;
		float emissionInterval = 0.01f; // 0.01초마다 파티클 생성 (100 파티클/초)
		
		while (particleEmissionTimer >= emissionInterval)
		{
			// 미사일 뒤쪽에서 연기 파티클 생성 (여러 개)
			for (int i = 0; i < 5; ++i) // 한 번에 5개씩 생성 (더 많이)
			{
				// 미사일 뒤쪽 위치 계산 (미사일의 길이를 고려)
				glm::vec3 smokePosition = position - direction * (height * 0.6f);
				
				// 약간의 랜덤 위치 변화 (더 큰 범위)
				smokePosition += glm::vec3(
					(rand() / float(RAND_MAX) - 0.5f) * 6.0f,
					(rand() / float(RAND_MAX) - 0.5f) * 3.0f,
					(rand() / float(RAND_MAX) - 0.5f) * 6.0f
				);
				
				// 연기 속도 (미사일 방향의 반대 + 더 큰 랜덤)
				glm::vec3 smokeVelocity = -direction * (speed * 0.3f) + glm::vec3(
					(rand() / float(RAND_MAX) - 0.5f) * 40.0f,
					(rand() / float(RAND_MAX) - 0.5f) * 20.0f,
					(rand() / float(RAND_MAX) - 0.5f) * 40.0f
				);
				
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
	
	// 연기 파티클 렌더링
	smokeTrail.render();
}

void Missile::RenderMissileLight(GLuint shaderProgramID, const glm::mat4& view, const glm::mat4& proj)
{
	if (!isActive) return;

	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
	GLint colorLoc = glGetUniformLocation(shaderProgramID, "aColor");
	GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
	GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");

	// 조명 위치 계산
	glm::vec3 lightPos = GetLightPosition();
	
	// 조명 모델 행렬 생성
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(lightIntensity)); // 펄스 효과로 크기 변화

	// 조명 색상 (밝은 노란색)
	glm::vec3 brightLightColor = lightColor * lightIntensity;

	// 행렬 및 색상 설정
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform3fv(colorLoc, 1, glm::value_ptr(brightLightColor));
	glUniform1f(useTextureLoc, 0.0f);
	glUniform1f(alphaValueLoc, 1.0f);

	// 블렌딩 설정 (발광 효과)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
	glDepthMask(GL_FALSE);

	// 조명 렌더링 (경고 수정: size_t를 GLsizei로 안전하게 변환)
	glBindVertexArray(lightVAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(lightIndices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 블렌딩 상태 복원
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}

void Missile::Launch(const glm::vec3& startPos, const glm::vec3& dir)
{
	position = startPos;
	direction = glm::normalize(dir);
	velocity = direction * speed;
	isActive = true;
	particleEmissionTimer = 0.0f;
	lightPulseTimer = 0.0f;
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
