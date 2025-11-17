#pragma once
#include "CommonInclude.h"
#include "Particle.h"
#include <vector>

class ParticleSystem
{
private:
	std::vector<Particle> particles;
	GLuint maxParticles;
	GLfloat emissionRate;
	GLfloat timeSinceLastEmission;

public:
	ParticleSystem(GLuint maxParticleCount = 1000);
	
	// 파티클 방출 (미사일 위치에서)
	void emitParticle(const glm::vec3& position, const glm::vec3& baseVelocity);
	
	// 연기 효과 업데이트
	void update(GLfloat deltaTime);
	
	// 파티클 렌더링
	void render();
	
	// 방출률 설정
	void setEmissionRate(GLfloat rate) { emissionRate = rate; }
	
	// 모든 파티클 제거
	void clear();
	
	// 활성 파티클 수 반환
	int getActiveParticleCount() const;
};