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
	
	// 생존 주기 계산 비율
	float lifeRatio = age / life;
	
	// 트레일 효과: 시간이 지날 때 서서히 사라지고 크기 확산
	color.a = (1.0f - lifeRatio) * 0.9f; // 최대 불투명도 0.9로 설정 (더 진하게)
	size = initialSize * (1.0f + lifeRatio * 1.5f); // 크기가 더 서서히 커짐 (트레일 밀집도 향상)
	
	// 중력이나 바람 효과 (감소) - 트레일이 더 일관되게 보이도록
	vel.y -= 0.2f * deltaTime; // 약간의 중력
	
	// 속도 감소 (공기 저항) - 트레일 지속성을 위해 감소량을 줄임
	vel *= 0.995f;
}
