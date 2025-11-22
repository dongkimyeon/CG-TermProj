#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"

class AA
{
public:
	AA();
	~AA();
	void Initialize();
	void InitBuffers();	
	void Update(float deltaTime);
	void Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale);
	
	void SetPosition(const glm::vec3& pos) { position = pos; }

	// 정적 메서드: 모든 AA 인스턴스가 공유하는 모델 로드
	static void LoadSharedModel();
	static void CleanupSharedModel();

private:
	// 인스턴스별 버퍼 (각 AA는 자신의 VAO/VBO/EBO 필요)
	GLuint vaoAA, vboAA, eboAA;
	glm::vec3 position;
	bool isDamaged = false;

	// 정적 공유 변수 (모든 AA 인스턴스가 공유)
	static FBXModel sharedModel;
	static bool modelLoaded;
};

