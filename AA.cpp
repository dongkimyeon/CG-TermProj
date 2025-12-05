#include "AA.h"

// 정적 변수 초기화
FBXModel AA::sharedModel;
bool AA::modelLoaded = false;
glm::vec3 AA::sharedBBoxMin = glm::vec3(0.0f);
glm::vec3 AA::sharedBBoxMax = glm::vec3(0.0f);
bool AA::sharedBBoxCalculated = false;

AA::AA()
	: position(glm::vec3(0.0f, 0.0f, 0.0f))
	, vaoAA(0), vboAA(0), eboAA(0)
	, boundingBoxVAO(0), boundingBoxVBO(0)
	, boundingBoxMin(glm::vec3(0.0f))
	, boundingBoxMax(glm::vec3(0.0f))
	, boundingBoxCalculated(false)
	, cachedModelScale(-1.0f)
	, health(100.0f)
	, maxHealth(100.0f)
{
}

AA::~AA()
{
	// 버퍼 정리 (각 인스턴스의 버퍼만 정리)
	if (vboAA != 0) glDeleteBuffers(1, &vboAA);
	if (eboAA != 0) glDeleteBuffers(1, &eboAA);
	if (vaoAA != 0) glDeleteVertexArrays(1, &vaoAA);
	
	// 바운딩 박스 버퍼 정리
	if (boundingBoxVBO != 0) glDeleteBuffers(1, &boundingBoxVBO);
	if (boundingBoxVAO != 0) glDeleteVertexArrays(1, &boundingBoxVAO);
}

void AA::Initialize()
{
	position = glm::vec3(0.0f, 10.0f, 0.0f);
	health = maxHealth;
	isDamaged = false;
}

void AA::LoadSharedModel()
{
	// 이미 로드되었으면 스킵
	if (modelLoaded) {
		std::cout << "AA 모델이 이미 로드되어 있습니다." << std::endl;
		return;
	}

	std::cout << "AA 공유 모델 로딩 시작..." << std::endl;

	// FBX 로드 - AA 폴더의 Models 하위 폴더에서 로드
	if (!LoadFBX("AA/Models/SK_East_AA_Shilka.FBX", &sharedModel)) {
		std::cerr << "AA FBX 로드 실패." << std::endl;
		return;
	}

	// 텍스처 로드
	sharedModel.textureList.resize(2);
	sharedModel.textureList[0] = new Texture("AA/Textures/MI_East_AA_Shilka_Main_BaseColor_0.png");
	sharedModel.textureList[0]->LoadTexture();

	sharedModel.textureList[1] = new Texture("AA/Textures/T_East_AA_Shilka_Damaged_D.png");
	sharedModel.textureList[1]->LoadTexture();

	sharedModel.normalMap = new Texture("AA/Textures/T_East_AA_Shilka_Damaged_N.png");
	sharedModel.normalMap->LoadTexture();

	modelLoaded = true;
	std::cout << "AA 공유 모델 로드 성공: " << sharedModel.vertices.size() << " vertices" << std::endl;
}

void AA::CleanupSharedModel()
{
	if (!modelLoaded) return;

	// 텍스처 정리
	for (auto* tex : sharedModel.textureList) {
		delete tex;
	}
	sharedModel.textureList.clear();

	if (sharedModel.normalMap) {
		delete sharedModel.normalMap;
		sharedModel.normalMap = nullptr;
	}

	modelLoaded = false;
	std::cout << "AA 공유 모델 정리 완료" << std::endl;
}

void AA::InitBuffers()
{
	if (!modelLoaded) {
		std::cerr << "경고: 공유 모델이 로드되지 않았습니다. LoadSharedModel()을 먼저 호출하세요." << std::endl;
		return;
	}

	const GLsizei stride = 11 * sizeof(GLfloat);
	
	// AA 버퍼 (인스턴스별)
	glGenVertexArrays(1, &vaoAA);
	glBindVertexArray(vaoAA);

	glGenBuffers(1, &vboAA);
	glBindBuffer(GL_ARRAY_BUFFER, vboAA);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &eboAA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboAA);
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

	// 공유 된 데이터를 각 인스턴스의 버퍼에 바인드
	UpdateModelBuffers(&sharedModel, vaoAA, vboAA, eboAA);

	glBindVertexArray(0);
	
	// 바운딩 박스 초기화
	InitBoundingBoxBuffers();
}

void AA::CalculateBoundingBox(float modelScale)
{
	// 스케일이 변경되지 않았으면 재계산 안함
	if (boundingBoxCalculated && cachedModelScale == modelScale) {
		return;
	}

	// 공유 바운딩 박스가 계산되지 않았으면 계산
	if (!sharedBBoxCalculated && modelLoaded && !sharedModel.vertices.empty()) {
		glm::vec3 minVert(FLT_MAX);
		glm::vec3 maxVert(-FLT_MAX);

		for (const auto& vertex : sharedModel.vertices) {
			minVert.x = (std::min)(minVert.x, vertex.x);
			minVert.y = (std::min)(minVert.y, vertex.y);
			minVert.z = (std::min)(minVert.z, vertex.z);

			maxVert.x = (std::max)(maxVert.x, vertex.x);
			maxVert.y = (std::max)(maxVert.y, vertex.y);
			maxVert.z = (std::max)(maxVert.z, vertex.z);
		}

		// 50% 크게 만들기
		glm::vec3 center = (minVert + maxVert) * 0.5f;
		glm::vec3 extent = (maxVert - minVert) * 0.5f * 1.5f; // 50% 증가

		sharedBBoxMin = center - extent;
		sharedBBoxMax = center + extent;
		sharedBBoxCalculated = true;
	}

	// 스케일 적용하여 인스턴스 바운딩 박스 설정
	boundingBoxMin = sharedBBoxMin * modelScale;
	boundingBoxMax = sharedBBoxMax * modelScale;
	boundingBoxCalculated = true;
	cachedModelScale = modelScale;
}

void AA::InitBoundingBoxBuffers()
{
	// 바운딩 박스 버퍼 생성 (한번만)
	glGenVertexArrays(1, &boundingBoxVAO);
	glGenBuffers(1, &boundingBoxVBO);
	
	glBindVertexArray(boundingBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
	
	// 미리 24개 점을 위한 공간 할당
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindVertexArray(0);
}

void AA::RenderBoundingBox(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, float modelScale)
{
	if (!modelLoaded) {
		return;
	}

	// 바운딩 박스 계산 (캐시 사용, 필요시에만 재계산)
	CalculateBoundingBox(modelScale);

	// 바운딩 박스의 8개 꼭짓점 계산
	glm::vec3 corners[8] = {
		glm::vec3(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z),
		glm::vec3(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z),
		glm::vec3(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z),
		glm::vec3(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z),
		glm::vec3(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z),
		glm::vec3(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z),
		glm::vec3(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z),
		glm::vec3(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z)
	};

	// 바운딩 박스의 12개 선분 (24개 점)
	glm::vec3 lines[24] = {
		// 아래 사각형
		corners[0], corners[1],
		corners[1], corners[2],
		corners[2], corners[3],
		corners[3], corners[0],
		// 위 사각형
		corners[4], corners[5],
		corners[5], corners[6],
		corners[6], corners[7],
		corners[7], corners[4],
		// 수직 선
		corners[0], corners[4],
		corners[1], corners[5],
		corners[2], corners[6],
		corners[3], corners[7]
	};

	// 모델 매트릭스 (위치만 적용)
	glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position);

	// 모델 매트릭스만 설정 (view, proj, color는 이미 설정됨)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

	// VAO/VBO 바인드 및 데이터 업데이트
	glBindVertexArray(boundingBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lines), lines);

	// 선 그리기
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 24);

	glBindVertexArray(0);
}

void AA::Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale)
{
	if (!modelLoaded) {
		return;
	}
	
	// 파괴된 AA는 렌더링하지 않음
	if (health <= 0.0f) {
		return;
	}

	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	GLint textureLoc = glGetUniformLocation(shaderID, "textureSampler");
	GLint normalMapLoc = glGetUniformLocation(shaderID, "normalMap");
	GLint alphaValueLoc = glGetUniformLocation(shaderID, "alphaValue");
	GLint useNormalMapLoc = glGetUniformLocation(shaderID, "useNormalMap");
	GLint useTextureLoc = glGetUniformLocation(shaderID, "useTexture");

	glUniform1i(useNormalMapLoc, sharedModel.normalMap != nullptr ? 1 : 0);

	// 모델 행렬: 위치 + 스케일 적용
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, position);
	modelMat = glm::scale(modelMat, glm::vec3(modelScale));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	glBindVertexArray(vaoAA);
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	for (size_t i = 0; i < sharedModel.meshes.size(); ++i) {
		const auto& meshInfo = sharedModel.meshes[i];

		glUniform1f(alphaValueLoc, 1.0f);
		glDepthMask(GL_TRUE);

		if (!isDamaged)
		{
			sharedModel.textureList[0]->UseTexture(0);
			glUniform1i(textureLoc, 0);
		}
		else
		{
			sharedModel.textureList[1]->UseTexture(0);
			glUniform1i(textureLoc, 0);
		}

		if (sharedModel.normalMap) {
			sharedModel.normalMap->UseTexture(1);
			glUniform1i(normalMapLoc, 1);
		}

		glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT,
			(void*)(meshInfo.indexStart * sizeof(GLuint)));
	}

	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void AA::Update(float deltaTime)
{
	// 업데이트 로직 추가 가능
}

void AA::TakeDamage(float damage)
{
	if (health <= 0.0f) {
		return; // 이미 파괴됨
	}

	health -= damage;
	
	if (health < maxHealth * 0.5f) {
		isDamaged = true; // 50% 이하면 데미지 텍스처 사용
	}
	
	if (health <= 0.0f) {
		health = 0.0f;
		std::cout << "AA 유닛 파괴됨!" << std::endl;
	}
	else {
		std::cout << "AA 피격! 남은 체력: " << health << "/" << maxHealth << std::endl;
	}
}

bool AA::CheckCollision(const glm::vec3& point, float modelScale) const
{
	if (health <= 0.0f) {
		return false; // 파괴된 AA는 충돌 안함
	}

	// 월드 공간 바운딩 박스 계산
	glm::vec3 worldMin = position + boundingBoxMin;
	glm::vec3 worldMax = position + boundingBoxMax;
	
	// AABB 충돌 검사
	return (point.x >= worldMin.x && point.x <= worldMax.x &&
	        point.y >= worldMin.y && point.y <= worldMax.y &&
	        point.z >= worldMin.z && point.z <= worldMax.z);
}

bool AA::CheckSphereCollision(const glm::vec3& center, float radius, float modelScale) const
{
	if (health <= 0.0f) {
		return false; // 파괴된 AA는 충돌 안함
	}

	// 월드 공간 바운딩 박스 계산
	glm::vec3 worldMin = position + boundingBoxMin;
	glm::vec3 worldMax = position + boundingBoxMax;
	
	// 구와 AABB의 최단 거리 계산
	glm::vec3 closestPoint;
	closestPoint.x = (std::max)(worldMin.x, (std::min)(center.x, worldMax.x));
	closestPoint.y = (std::max)(worldMin.y, (std::min)(center.y, worldMax.y));
	closestPoint.z = (std::max)(worldMin.z, (std::min)(center.z, worldMax.z));
	
	// 거리 제곱 계산 (벡터 차이의 제곱 길이)
	glm::vec3 diff = closestPoint - center;
	float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
	
	return distanceSquared <= (radius * radius);
}