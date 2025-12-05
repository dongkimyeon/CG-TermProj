#include "Particle.h"
#include <cmath>
#include <iostream> // Debugging 추가

Particle::Particle()
	: pos(0.0f), vel(0.0f), color(1.0f), 
	  life(1.0f), age(0.0f), rot(0.0f), angularVel(0.0f),
	  size(1.0f), initialSize(1.0f)
{
}

void Particle::initialize(const glm::vec3& position, const glm::vec3& velocity, 
						  const glm::vec4& initialColor, GLfloat lifeTime, 
						  GLfloat particleSize, GLfloat rotSpeed)
{
	pos = position;
	vel = velocity;
	color = initialColor;
	life = lifeTime;
	age =0.0f;
	rot =0.0f;
	angularVel = rotSpeed;
	size = particleSize;
	initialSize = particleSize;
}

void Particle::update(GLfloat deltaTime)
{
	if (!isAlive()) return;

	// age first so turbulence uses updated age
	age += deltaTime;

	float lifeRatio = age / life;

	// Store old position for debugging
	glm::vec3 oldPos = pos;

	// Buoyancy: very strong upward acceleration for smoke
	vel.y += 8.0f * deltaTime; // 더욱 강력하게 증가

	// Small turbulence so particles don't move in straight lines
	float jitterX = std::sin(age *10.0f + pos.z *0.5f) *0.1f;
	float jitterZ = std::cos(age *8.0f + pos.x *0.4f) *0.1f;
	vel.x += jitterX * deltaTime;
	vel.z += jitterZ * deltaTime;

	// Apply velocity - 이것이 핵심!
	pos += vel * deltaTime;

	// Debug output (first few frames only)
	static int debugCount = 0;
	if (debugCount < 5 && age < 0.5f) {
		std::cout << "Particle Update - Delta: " << deltaTime 
				  << " Old Y: " << oldPos.y 
				  << " New Y: " << pos.y 
				  << " Vel Y: " << vel.y << std::endl;
		debugCount++;
	}

	// Rotation
	rot += angularVel * deltaTime;

	// Size: grow initially then gently shrink toward the end for a puff effect
	size = initialSize * (1.0f + lifeRatio *1.8f - lifeRatio * lifeRatio *1.2f);
	if (size <0.01f) size =0.01f;

	// Alpha: use smoother falloff (gamma) for more natural fade
	float alphaBase =0.9f;
	color.a = alphaBase * std::powf(1.0f - lifeRatio,1.6f);

	// Color shift: become slightly lighter (smoke brightening) as it ages
	glm::vec3 target(0.88f,0.88f,0.88f);
	float t = std::min(1.0f, lifeRatio *0.6f);
	color.r = color.r * (1.0f - t) + target.r * t;
	color.g = color.g * (1.0f - t) + target.g * t;
	color.b = color.b * (1.0f - t) + target.b * t;

	// Minimal damping for smoke to rise freely
	vel *= 0.99f; // 감쇠를 약간 증가시켜 안정성 확보
}
