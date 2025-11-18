#include "ParticleSystem.h"
#include <random>
#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem(GLuint maxParticleCount)
	: maxParticles(maxParticleCount), emissionRate(50.0f), timeSinceLastEmission(0.0f),
	  VAO(0), VBO(0)
{
	particles.reserve(maxParticles);
	vertexData.reserve(maxParticles * 7); // 위치(3) + 색상(4) per vertex
	setupBuffers();
	std::cout << "ParticleSystem 생성됨 - 최대 파티클: " << maxParticles << std::endl;
}

ParticleSystem::~ParticleSystem()
{
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
	if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void ParticleSystem::setupBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	// 동적 버퍼로 설정
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 7 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
	
	// 위치 (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	
	// 색상 (location 4) - aColor 위치
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	
	glBindVertexArray(0);
	
	std::cout << "ParticleSystem VAO/VBO 설정 완료" << std::endl;
}

void ParticleSystem::emitParticle(const glm::vec3& position, const glm::vec3& baseVelocity)
{
	// 랜덤 생성기 (C++14 호환)
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
	static std::uniform_real_distribution<float> lifeDis(2.0f, 4.0f);
	static std::uniform_real_distribution<float> sizeDis(1.0f, 3.0f);
	
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
		// 연기 파티클 초기화 - 더 보이기 쉽게 수정
		glm::vec3 randomVel = baseVelocity + glm::vec3(
			dis(gen) * 5.0f,  // x축 랜덤
			std::abs(dis(gen)) * 2.0f + 0.5f,  // y축 랜덤 (항상 위쪽)
			dis(gen) * 5.0f   // z축 랜덤
		);
		
		// 연기 색상 (훨씬 밝게 수정)
		float grayVariation = 0.8f + dis(gen) * 0.2f; // 0.8 ~ 1.0 범위 (밝은 회색)
		glm::vec4 smokeColor = glm::vec4(grayVariation, grayVariation, grayVariation, 1.0f);
		
		particle->initialize(
			position + glm::vec3(dis(gen) * 2.0f, dis(gen) * 1.0f, dis(gen) * 2.0f), // 약간의 위치 변화
			randomVel,
			smokeColor,
			lifeDis(gen),  // 2-4초 생명
			sizeDis(gen),  // 1.0-3.0 크기
			dis(gen) * 180.0f  // 회전 속도
		);
		
		static int debugCount = 0;
		debugCount++;
		if (debugCount % 100 == 0) {
			std::cout << "파티클 생성됨! 총 " << particles.size() << "개, 활성: " << getActiveParticleCount() << "개" << std::endl;
		}
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
	if (cleanupTimer > 2.0f) // 2초마다 정리
	{
		int beforeSize = particles.size();
		particles.erase(
			std::remove_if(particles.begin(), particles.end(),
				[](const Particle& p) { return p.isDead(); }),
			particles.end()
		);
		int afterSize = particles.size();
		if (beforeSize != afterSize) {
			std::cout << "죽은 파티클 정리: " << (beforeSize - afterSize) << "개 제거" << std::endl;
		}
		cleanupTimer = 0.0f;
	}
	
	// 버텍스 데이터 업데이트
	updateVertexData();
}

void ParticleSystem::updateVertexData()
{
	vertexData.clear();
	
	for (const auto& particle : particles)
	{
		if (particle.isAlive())
		{
			// 위치 데이터
			vertexData.push_back(particle.pos.x);
			vertexData.push_back(particle.pos.y);
			vertexData.push_back(particle.pos.z);
			
			// 색상 데이터 (RGBA)
			vertexData.push_back(particle.color.r);
			vertexData.push_back(particle.color.g);
			vertexData.push_back(particle.color.b);
			vertexData.push_back(particle.color.a);
		}
	}
}

void ParticleSystem::render()
{
	if (particles.empty()) return;
	
	int activeCount = getActiveParticleCount();
	if (activeCount == 0) return;
	
	// 현재 셰이더 프로그램 가져오기
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	
	if (currentProgram == 0) return; // 셰이더 프로그램이 없으면 렌더링하지 않음
	
	// 상태 저장
	GLboolean depthMask;
	GLboolean blend;
	GLboolean cullFace;
	GLboolean pointSprite;
	
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
	glGetBooleanv(GL_BLEND, &blend);
	glGetBooleanv(GL_CULL_FACE, &cullFace);
	glGetBooleanv(GL_PROGRAM_POINT_SIZE, &pointSprite);
	
	// 파티클 렌더링을 위한 OpenGL 상태 설정
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // 깊이 버퍼 쓰기 비활성화
	glDisable(GL_CULL_FACE); // 면 제거 비활성화
	
	// 점 크기 설정
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	// 셰이더 uniform 설정
	GLint useTextureLoc = glGetUniformLocation(currentProgram, "useTexture");
	GLint alphaValueLoc = glGetUniformLocation(currentProgram, "alphaValue");
	
	GLfloat savedUseTexture = 1.0f;
	GLfloat savedAlpha = 1.0f;
	
	if (useTextureLoc != -1) {
		glGetUniformfv(currentProgram, useTextureLoc, &savedUseTexture);
		glUniform1f(useTextureLoc, 0.0f); // 텍스처 사용 안 함
	}
	if (alphaValueLoc != -1) {
		glGetUniformfv(currentProgram, alphaValueLoc, &savedAlpha);
		glUniform1f(alphaValueLoc, 1.0f); // 기본 알파값
	}
	
	// 버퍼 업데이트 및 렌더링
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	if (!vertexData.empty()) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(GLfloat), vertexData.data());
		
		// 모든 파티클을 한 번에 점으로 렌더링
		int activeParticles = vertexData.size() / 7; // 7 floats per particle
		glDrawArrays(GL_POINTS, 0, activeParticles);
		
		static int debugRenderCount = 0;
		debugRenderCount++;
		if (debugRenderCount % 300 == 0) {
			std::cout << "파티클 렌더링: " << activeParticles << "개 점으로 렌더링됨" << std::endl;
		}
	}
	
	glBindVertexArray(0);
	
	// 상태 복원
	if (useTextureLoc != -1) {
		glUniform1f(useTextureLoc, savedUseTexture);
	}
	if (alphaValueLoc != -1) {
		glUniform1f(alphaValueLoc, savedAlpha);
	}
	
	// OpenGL 상태 복원
	if (!pointSprite) glDisable(GL_PROGRAM_POINT_SIZE);
	if (cullFace) glEnable(GL_CULL_FACE);
	glDepthMask(depthMask);
	if (!blend) glDisable(GL_BLEND);
}

void ParticleSystem::clear()
{
	particles.clear();
	vertexData.clear();
	std::cout << "ParticleSystem 클리어됨" << std::endl;
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