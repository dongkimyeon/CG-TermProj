#pragma once
#include "ParticleSystem.h"
#include "CommonInclude.h"

extern ParticleSystem* s_persistentParticles;

// Ensure the persistent particle system exists and is initialized.
void EnsurePersistentParticles();

// Update and render helpers for the persistent particle system.
void UpdatePersistentParticles(float dt);
void RenderPersistentParticles(const glm::mat4& view, const glm::mat4& proj);

// Optional cleanup (not strictly necessary depending on program lifecycle)
void CleanupPersistentParticles();
