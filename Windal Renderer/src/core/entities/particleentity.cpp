#include "core/entities/particleentity.h"
#include "graphics/particlesystem.h"

ParticleEntity::ParticleEntity(uint32_t count)
	: Entity("Particle")
{
	mParticleSystem = std::make_shared<ParticleSystem>(count);
}

ParticleEntity::~ParticleEntity()
{
}

void ParticleEntity::UpdateSelf(double delta)
{
	mParticleSystem->Update();
}

void ParticleEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushParticleSystem(mParticleSystem, transform);
}

void ParticleEntity::RenderImguiSelf()
{
}