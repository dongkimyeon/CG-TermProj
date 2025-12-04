#include "CannonBullet.h"
#include "ParticleManager.h"
#include "Ground.h"

CannonBullet::CannonBullet()
	: position(0.0f), direction(0.0f, 0.0f, 1.0f), speed(2500.0f), life(0.0f), maxLife(1.0f), 
	  active(false), smokeTrail(1000), trailSpacing(3.0f), mGround(nullptr)
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
	maxLife = 10.0f;
	active = true;
	smokeTrail.clear();
}

void CannonBullet::Update(float dt)
{
	if (!active) return;
	
	glm::vec3 prevPos = position;
	position += -direction * speed * dt;
	life += dt;

	// trail 생성 (선형 궤적만 남김)
	float dist = glm::length(position - prevPos);
	int nTrail = std::max(1, (int)(dist / trailSpacing));

	for (int i = 0; i < nTrail; ++i) {
		float t = nTrail > 1 ? float(i) / float(nTrail - 1) : 0.0f;
		glm::vec3 trailPos = glm::mix(prevPos, position, t);
		smokeTrail.emitParticle(trailPos, glm::vec3(0, 0, 0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 3.8f, 1.0f);
	}
	smokeTrail.update(dt);
	
	// 지형 충돌 검사
	float groundHeight = -2.0f; // Default ground level
	
	if (mGround)
	{
		// Get accurate terrain height at bullet position
		groundHeight = mGround->GetHeightAt(position.x, position.z);
	}
	
	// Check collision with terrain (with small margin)
	if (position.y <= (groundHeight + 0.5f))
	{
		// Transfer particles to persistent manager before deleting bullet
		EnsurePersistentParticles();
		std::vector<Particle> stolen = smokeTrail.stealParticles();
		if (!stolen.empty()) {
			s_persistentParticles->addParticles(std::move(stolen));
		}
		active = false;
	}
	
	// Check if bullet exceeded max life time or is out of bounds
	if (life > maxLife || glm::length(position) > 5000.0f)
	{
		// Transfer remaining particles before deactivating
		EnsurePersistentParticles();
		std::vector<Particle> stolen = smokeTrail.stealParticles();
		if (!stolen.empty()) {
			s_persistentParticles->addParticles(std::move(stolen));
		}
		active = false;
	}
}

void CannonBullet::Render(const glm::mat4& view, const glm::mat4& proj)
{
	smokeTrail.render(view, proj);
	// 탄환 자체는 렌더링 안함(파티클만)
}
