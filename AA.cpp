#include "AA.h"

// 정적 변수 초기화
FBXModel AA::sharedModel;
bool AA::modelLoaded = false;

AA::AA()
	: position(glm::vec3(0.0f, 0.0f, 0.0f))
	, vaoAA(0), vboAA(0), eboAA(0)
{
}

AA::~AA()
{
	// 버퍼 정리 (각 인스턴스의 버퍼만 삭제)
	if (vboAA != 0) glDeleteBuffers(1, &vboAA);
	if (eboAA != 0) glDeleteBuffers(1, &eboAA);
	if (vaoAA != 0) glDeleteVertexArrays(1, &vaoAA);
}

void AA::Initialize()
{
	position = glm::vec3(0.0f, 10.0f, 0.0f);
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

	// 공유 모델 데이터를 이 인스턴스의 버퍼에 업로드
	UpdateModelBuffers(&sharedModel, vaoAA, vboAA, eboAA);

	glBindVertexArray(0);
}

void AA::Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale)
{
	if (!modelLoaded) {
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