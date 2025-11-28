#include "CannonBullet.h"

CannonBullet::CannonBullet()
	: position(0.0f), direction(0.0f, 0.0f, 1.0f), speed(2500.0f), life(0.0f), maxLife(1.0f), active(false), smokeTrail(100), trailSpacing(3.0f)
{
	smokeTrail.initialize();
}

CannonBullet::~CannonBullet() {}

void CannonBullet::Launch(const glm::vec3& startPos, const glm::vec3& dir, float spd)
{
	position = startPos;
	direction = glm::normalize(dir);
	speed = spd;
	life = 0.0f;
	maxLife = 1.0f; //1초 후 소멸
	active = true;
	smokeTrail.clear();
}

void CannonBullet::Update(float dt)
{
	if (!active) return;
	glm::vec3 prevPos = position;
	position += -direction * speed * dt;
	life += dt;

	// trail 생성 (아주 빠르게 직선)
	float dist = glm::length(position - prevPos);
	int nTrail = std::max(1, (int)(dist / trailSpacing));

	for (int i = 0; i < nTrail; ++i) {
		float t = nTrail > 1 ? float(i) / float(nTrail - 1) : 0.0f;
		glm::vec3 trailPos = glm::mix(prevPos, position, t);
		smokeTrail.emitParticle(trailPos, glm::vec3(0, 0, 0));
	}
	smokeTrail.update(dt);
	if (life > maxLife || position.y < 0.0f || glm::length(position) >3000.0f) active = false;
}

void CannonBullet::Render(const glm::mat4& view, const glm::mat4& proj)
{
	smokeTrail.render(view, proj);
	// 탄환 자체는 보이지 않게(스모크만)
}
