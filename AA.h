#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"

class AA
{
public:
	AA();
	~AA();
	void Initialize();
	void LoadModel();
	void Update(float deltaTime);
	void Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale);
	void InitBuffers();	

private:
	FBXModel aaModel;
	GLuint vaoAA, vboAA, eboAA;
	glm::vec3 position;
	bool isDamaged = false;
};

