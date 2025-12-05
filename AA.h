#pragma once
#include "CommonInclude.h"
#include "FBXModel.h"
#include "ParticleSystem.h"

class AA
{
public:
	AA();
	~AA();
	void Initialize();
	void InitBuffers();	
	void Update(float deltaTime);
	void Render(GLuint shaderID, bool wireframeMode, float glassAlpha, float modelScale);
	void RenderBoundingBox(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, float modelScale);
	
	// 연기 파티클 렌더링 (모델과 별도로)
	void RenderSmoke(const glm::mat4& view, const glm::mat4& proj);
	
	void SetPosition(const glm::vec3& pos) { position = pos; }
	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetBoundingBoxMin() const { return boundingBoxMin; }
	glm::vec3 GetBoundingBoxMax() const { return boundingBoxMax; }
	
	// 체력 및 피해 시스템
	void TakeDamage(float damage);
	float GetHealth() const { return health; }
	float GetMaxHealth() const { return maxHealth; }
	bool IsAlive() const { return health > 0.0f; }
	bool IsDestroyed() const { return health <= 0.0f; }
	
	// 충돌 검사
	bool CheckCollision(const glm::vec3& point, float modelScale) const;
	bool CheckSphereCollision(const glm::vec3& center, float radius, float modelScale) const;
	
	// 공유 메서드: 모든 AA 인스턴스가 공유하는 한 번 로드
	static void LoadSharedModel();
	static void CleanupSharedModel();

private:
	void CalculateBoundingBox(float modelScale);
	void InitBoundingBoxBuffers();
	void EmitSmokeParticles(float deltaTime);

	// 인스턴스 버퍼 (각 AA가 자신의 VAO/VBO/EBO 필요)
	GLuint vaoAA, vboAA, eboAA;
	glm::vec3 position;
	bool isDamaged = false;
	
	// 체력 시스템
	float health;
	float maxHealth;
	
	// 파괴 상태 및 연기 시스템
	bool isDestroyed = false;
	ParticleSystem* smokeSystem;
	float smokeEmitTimer = 0.0f;
	float smokeDuration = 0.0f; // 연기가 지속되는 시간 (무한히 나오도록 설정)
	const float smokeEmitInterval = 0.05f; // 연기 방출 간격
	
	// 연기 업데이트 FPS 제한 (새로 추가)
	float smokeUpdateTimer = 0.0f;
	const float smokeUpdateInterval = 1.0f / 30.0f; // 30 FPS로 제한 (약 0.033초)
	
	// 바운딩 박스 데이터
	glm::vec3 boundingBoxMin;
	glm::vec3 boundingBoxMax;
	GLuint boundingBoxVAO, boundingBoxVBO;
	bool boundingBoxCalculated;
	float cachedModelScale;
	
	// 공유 모델 데이터 (모든 AA 인스턴스가 공유)
	static FBXModel sharedModel;
	static bool modelLoaded;
	
	// 공유 바운딩 박스 데이터 (모든 AA가 같은 모델이므로)
	static glm::vec3 sharedBBoxMin;
	static glm::vec3 sharedBBoxMax;
	static bool sharedBBoxCalculated;
};

