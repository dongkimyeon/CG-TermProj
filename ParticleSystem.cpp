#include "ParticleSystem.h"
#include <random>
#include <algorithm>

ParticleSystem::ParticleSystem(GLuint maxParticleCount)
	: maxParticles(maxParticleCount), emissionRate(50.0f), timeSinceLastEmission(0.0f)
{
	particles.reserve(maxParticles);
}

void ParticleSystem::emitParticle(const glm::vec3& position, const glm::vec3& baseVelocity)
{
	// 랜덤 생성기 (C++14 호환)
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
	static std::uniform_real_distribution<float> lifeDis(1.5f, 3.5f);
	static std::uniform_real_distribution<float> sizeDis(0.3f, 1.2f);
	
	// 죽은 파티클 찾기 또는 새 파티클 추가
	Particle* particle = nullptr;
	
	// 죽은 파티클 재사용
	for (auto& p : particles)
	{
		if (p.isDead())
		{
			particle = &p;
			break;
		}
	}
	
	// 새 파티클 생성 (최대 개수 내에서)
	if (particle == nullptr && particles.size() < maxParticles)
	{
		particles.emplace_back();
		particle = &particles.back();
	}
	
	if (particle != nullptr)
	{
		// 연기 파티클 초기화 - 좀 더 자연스러운 랜덤 값
		glm::vec3 randomVel = baseVelocity + glm::vec3(
			dis(gen) * 1.5f,  // x축 랜덤
			std::abs(dis(gen)) * 0.8f + 0.2f,  // y축 랜덤 (항상 위쪽)
			dis(gen) * 1.5f   // z축 랜덤
		);
		
		// 연기 색상 (약간의 랜덤 변화)
		float grayVariation = 0.4f + dis(gen) * 0.3f; // 0.1 ~ 0.7 범위
		glm::vec4 smokeColor = glm::vec4(grayVariation, grayVariation, grayVariation, 0.7f);
		
		particle->initialize(
			position + glm::vec3(dis(gen) * 0.2f, dis(gen) * 0.1f, dis(gen) * 0.2f), // 약간의 위치 변화
			randomVel,
			smokeColor,
			lifeDis(gen),  // 1.5-3.5초 생명
			sizeDis(gen),  // 0.3-1.2 크기
			dis(gen) * 180.0f  // 회전 속도
		);
	}
}

void ParticleSystem::update(GLfloat deltaTime)
{
	// 모든 파티클 업데이트
	for (auto& particle : particles)
	{
		if (particle.isAlive())
		{
			particle.update(deltaTime);
		}
	}
	
	timeSinceLastEmission += deltaTime;
	
	// 죽은 파티클 정리 (가끔씩만 실행하여 성능 최적화)
	static float cleanupTimer = 0.0f;
	cleanupTimer += deltaTime;
	if (cleanupTimer > 1.0f) // 1초마다 정리
	{
		particles.erase(
			std::remove_if(particles.begin(), particles.end(),
				[](const Particle& p) { return p.isDead(); }),
			particles.end()
		);
		cleanupTimer = 0.0f;
	}
}

void ParticleSystem::render()
{
	// 파티클 렌더링은 렌더링 시스템에 따라 구현
	// 현재는 기본 구조만 제공
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // 깊이 버퍼 쓰기 비활성화 (투명도 정렬 개선)
	
	for (const auto& particle : particles)
	{
		if (particle.isAlive())
		{
			// 여기에 파티클 렌더링 코드 추가
			// 보통 billboard quad를 사용하여 연기 텍스처를 렌더링
			// glPushMatrix();
			// glTranslatef(particle.pos.x, particle.pos.y, particle.pos.z);
			// glRotatef(particle.rot, 0, 0, 1);
			// glScalef(particle.size, particle.size, 1.0f);
			// glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
			// // 텍스처 바인딩 및 quad 렌더링
			// glPopMatrix();
		}
	}
	
	glDepthMask(GL_TRUE); // 깊이 버퍼 쓰기 다시 활성화
	glDisable(GL_BLEND);
}

void ParticleSystem::clear()
{
	particles.clear();
}

int ParticleSystem::getActiveParticleCount() const
{
	int count = 0;
	for (const auto& particle : particles)
	{
		if (particle.isAlive())
			count++;
	}
	return count;
}