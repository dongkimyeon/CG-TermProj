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
	GLuint particleShaderProgram=0;
	std::vector<glm::vec4> instanceData; // xyz position, w scale

public:
	ParticleSystem(size_t maxCount=1000);
	void initialize();
	void emitParticle(const glm::vec3& pos, const glm::vec3& vel);
	void update(float dt);
	void render(const glm::mat4& view, const glm::mat4& proj);
	void clear();

private:
	void setupCubeGeometry();
	void updateInstanceBuffer();
};
