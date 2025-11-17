#include "Particle.h"

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
	age = 0.0f;
	rot = 0.0f;
	angularVel = rotSpeed;
	size = particleSize;
	initialSize = particleSize;
}

void Particle::update(GLfloat deltaTime)
{
	if (!isAlive()) return;

	// 나이 증가
	age += deltaTime;
	
	// 위치 업데이트
	pos += vel * deltaTime;
	
	// 회전 업데이트
	rot += angularVel * deltaTime;
	
	// 생명 주기 비율 계산
	float lifeRatio = age / life;
	
	// 연기 효과: 시간에 따라 투명도 감소 및 크기 증가
	color.a = (1.0f - lifeRatio) * 0.8f; // 최대 투명도를 0.8로 제한
	size = initialSize * (1.0f + lifeRatio * 2.0f); // 크기가 점점 커짐 (연기 확산)
	
	// 중력이나 바람 효과 (선택적)
	vel.y -= 0.5f * deltaTime; // 약간의 중력
	
	// 속도 감소 (공기 저항)
	vel *= 0.98f;
}
