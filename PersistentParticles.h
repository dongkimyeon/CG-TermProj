#pragma once
#include "CommonInclude.h"
#include "ParticleSystem.h"

// Expose persistent particle system helpers used by main loop and missiles
extern ParticleSystem* s_persistentParticles;

void EnsurePersistentParticles();
void UpdatePersistentParticles(float deltaTime);
void RenderPersistentParticles(const glm::mat4& view, const glm::mat4& proj);
void CleanupPersistentParticles();
