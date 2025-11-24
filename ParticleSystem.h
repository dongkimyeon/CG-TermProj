#pragma once
#include "CommonInclude.h"
#include "Particle.h"
#include <vector>
#include "Mapping/shader.hpp"

class ParticleSystem {
private:
	std::vector<Particle> particles;
	size_t maxParticles;

	GLuint cubeVAO=0, cubeVBO=0, cubeEBO=0;
	GLuint instanceVBO=0;
	GLuint instanceColorVBO=0; // 추가: 인스턴스별 색상 버퍼
	GLuint particleShaderProgram=0;
	std::vector<glm::vec4> instanceData; // xyz position, w scale
	std::vector<glm::vec4> instanceColorData; // rgba per-instance

public:
	// 더 큰 파티클과 더 긴 수명을 위해 파티클 수를 2500에서 4000으로 증가
	ParticleSystem(size_t maxCount=4000);
	void initialize();
	void emitParticle(const glm::vec3& pos, const glm::vec3& vel);
	void update(float dt);
	void render(const glm::mat4& view, const glm::mat4& proj);
	void clear();

private:
	void setupCubeGeometry();
	void updateInstanceBuffer();
};
