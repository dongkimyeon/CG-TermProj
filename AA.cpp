#include "AA.h"


AA::AA()
	: position(glm::vec3(0.0f, 0.0f, 0.0f))
	, vaoAA(0), vboAA(0), eboAA(0)
{
}

AA::~AA()
{
	// 버퍼 정리
	if (vboAA != 0) glDeleteBuffers(1, &vboAA);
	if (eboAA != 0) glDeleteBuffers(1, &eboAA);
	if (vaoAA != 0) glDeleteVertexArrays(1, &vaoAA);
}


void AA::Initialize()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
}



void AA::LoadModel()
{
	// FBX 로드 - AA 폴더의 Models 하위 폴더에서 로드
	if (!LoadFBX("AA/Models/SK_East_AA_Shilka.FBX", &aaModel)) {
		std::cerr << "AA FBX 로드 실패." << std::endl;
	}
	else {
		aaModel.textureList.resize(2);  
		aaModel.textureList[0] = new Texture("AA/Textures/MI_East_AA_Shilka_Main_BaseColor_0.png");
		aaModel.textureList[0]->LoadTexture();

	
		aaModel.textureList[1] = new Texture("AA/Textures/T_East_AA_Shilka_Damaged_D.png");
		aaModel.textureList[1]->LoadTexture();

		aaModel.normalMap = new Texture("AA/Textures/T_East_AA_Shilka_Damaged_N.png");
		aaModel.normalMap->LoadTexture();

		InitBuffers();


		UpdateModelBuffers(&aaModel, vaoAA, vboAA, eboAA);

		std::cout << "AA 로드 성공: " << aaModel.vertices.size() << " vertices" << std::endl;
	}
}

void AA::InitBuffers()
{
	const GLsizei stride = 11 * sizeof(GLfloat);
	// AA 버퍼
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
}

void AA::Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale)
{
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	GLint textureLoc = glGetUniformLocation(shaderID, "textureSampler");
	GLint normalMapLoc = glGetUniformLocation(shaderID, "normalMap");
	GLint alphaValueLoc = glGetUniformLocation(shaderID, "alphaValue");
	GLint useNormalMapLoc = glGetUniformLocation(shaderID, "useNormalMap");
	GLint useTextureLoc = glGetUniformLocation(shaderID, "useTexture");

	glUniform1i(useNormalMapLoc, aaModel.normalMap != nullptr ? 1 : 0);

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::scale(modelMat, glm::vec3(modelScale));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	glBindVertexArray(vaoAA);
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	for (size_t i = 0; i < aaModel.meshes.size(); ++i) {
		const auto& meshInfo = aaModel.meshes[i];

	
		glUniform1f(alphaValueLoc, 1.0f);
		glDepthMask(GL_TRUE);

		if (!isDamaged)
		{
			aaModel.textureList[0]->UseTexture(0);
			glUniform1i(textureLoc, 0);
		}
		else
		{
			aaModel.textureList[1]->UseTexture(0);
			glUniform1i(textureLoc, 0);
		}
	

		if (aaModel.normalMap) {
			aaModel.normalMap->UseTexture(1);
			glUniform1i(normalMapLoc, 1);
		}

		glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT,
			(void*)(meshInfo.indexStart * sizeof(GLuint)));
	}

	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}