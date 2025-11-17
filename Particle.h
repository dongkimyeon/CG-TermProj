#pragma once
#include "CommonInclude.h"

class Particle
{
public:
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec4 color;

	GLfloat life;
	GLfloat age;
	GLfloat rot;
	GLfloat angularVel;
	GLfloat size;
	GLfloat initialSize;

	// 생성자
	Particle();
	
	// 파티클 초기화 (연기 효과용)
	void initialize(const glm::vec3& position, const glm::vec3& velocity, 
					const glm::vec4& initialColor, GLfloat lifeTime, 
					GLfloat initialSize, GLfloat rotSpeed);

	// 업데이트 함수
	void update(GLfloat deltaTime);

	// 파티클이 살아있는지 확인
	bool isAlive() const { return age < life; }

	// 파티클이 죽었는지 확인
	bool isDead() const { return age >= life; }
};

