#include "PersistentParticles.h"
#include <iostream>

ParticleSystem* s_persistentParticles = nullptr;

void EnsurePersistentParticles()
{
 if (!s_persistentParticles) {
 s_persistentParticles = new ParticleSystem(8000);
 s_persistentParticles->initialize();
 }
}

void UpdatePersistentParticles(float deltaTime)
{
 if (s_persistentParticles) s_persistentParticles->update(deltaTime);
}

void RenderPersistentParticles(const glm::mat4& view, const glm::mat4& proj)
{
 if (s_persistentParticles) s_persistentParticles->render(view, proj);
}

void CleanupPersistentParticles()
{
 if (s_persistentParticles) {
 delete s_persistentParticles;
 s_persistentParticles = nullptr;
 }
}
