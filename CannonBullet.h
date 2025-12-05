#pragma once
#include "CommonInclude.h"
#include "ParticleSystem.h"

class CannonBullet {
public:
 CannonBullet();
 ~CannonBullet();

 void Launch(const glm::vec3& startPos, const glm::vec3& dir, float speed = 2500.0f);
 void Update(float dt);
 void Render(const glm::mat4& view, const glm::mat4& proj);
 bool IsActive() const { return active; }
 glm::vec3 GetPosition() const { return position; }
 void Deactivate() { active = false; }
 
 // Set ground reference for terrain collision
 void SetGround(class Ground* ground) { mGround = ground; }

private:
 glm::vec3 position;
 glm::vec3 direction;
 float speed;
 float life;
 float maxLife;
 bool active;
 ParticleSystem smokeTrail;
 float trailSpacing;
 
 // Terrain reference for collision detection
 class Ground* mGround;
};
