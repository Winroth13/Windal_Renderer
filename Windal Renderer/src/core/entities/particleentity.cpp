#include "core/entities/particleentity.h"
#include "graphics/particlesystem.h"
#include "graphics/textures/texture2d.h"

#include "core/imguiflags.h"
#include "imgui/imgui.h"

#include "core/logger.h"

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
	mParticleSystem->Update(delta);
}

void ParticleEntity::RenderSelf(RenderServer& renderServer)
{
	if (mParticleSystem->GetColorTexture() && mParticleSystem->GetAlphaTexture())
		renderServer.PushParticleSystem(mParticleSystem, transform);
}

void ParticleEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Particle System", TREE_NODE_FLAGS))
	{
		if (ImGui::Button("Reset"))
		{
			mParticleSystem->Reset();
		}
		
		ImGui::Text("Particle Count: %d", mParticleSystem->GetCount());

		/* Lifetime */
		{
			float lifeTime = mParticleSystem->GetLifeTime();
			if (ImGui::DragFloat("Lifetime", &lifeTime, 0.01f, 0, FLT_MAX))
			{
				mParticleSystem->SetLifeTime(lifeTime);
				mParticleSystem->Reset();
			}
		}

		/* Spawn Radius */
		{
			float spawnRadius = mParticleSystem->GetSpawnRadius();
			if (ImGui::DragFloat("Spawn Radius", &spawnRadius, 0.01f, 0, FLT_MAX))
			{
				mParticleSystem->SetSpawnRadius(spawnRadius);
			}
		}

		/* Velocity */
		{
			float velocity = mParticleSystem->GetVelocity();
			if (ImGui::DragFloat("Velocity", &velocity, 0.01f, 0, FLT_MAX))
			{
				mParticleSystem->SetVelocity(velocity);
			}
		}

		/* Start Scale */
		{
			float startScale = mParticleSystem->GetStartScale();
			if (ImGui::DragFloat("Start Scale", &startScale, 0.01f, 0, FLT_MAX))
			{
				mParticleSystem->SetStartScale(startScale);
			}
		}

		/* End Scale */
		{
			float endScale = mParticleSystem->GetEndScale();
			if (ImGui::DragFloat("End Scale", &endScale, 0.01f, 0, FLT_MAX))
			{
				mParticleSystem->SetEndScale(endScale);
			}
		}

		/* Animation */
		{
			bool isAnimated = mParticleSystem->IsAnimated();
			if (ImGui::Checkbox("Animated", &isAnimated))
			{
				mParticleSystem->SetAnimated(isAnimated);
			}

			if (isAnimated)
			{
				/* Atlas Speed */
				{
					float atlasSpeed = mParticleSystem->GetAtlasSpeed();
					if (ImGui::DragFloat("Atlas Speed", &atlasSpeed, 0.01f, 0, FLT_MAX))
					{
						mParticleSystem->SetAtlasSpeed(atlasSpeed);
					}
				}

				/* Atlas Size */
				{
					float atlasWidth = mParticleSystem->GetAtlasWidth();
					float atlasHeight = mParticleSystem->GetAtlasHeight();
					int atlasSize[2] = { (int)atlasWidth, (int)atlasHeight };
					if (ImGui::DragInt2("Atlas Size", &atlasSize[0], 0.05f, 1, INT32_MAX))
					{
						mParticleSystem->SetAtlasWidth((float)atlasSize[0]);
						mParticleSystem->SetAtlasHeight((float)atlasSize[1]);
					}
				}
			}
		}

		/* Desaturate */
		{
			bool isDesaturate = mParticleSystem->IsDesaturate();
			if (ImGui::Checkbox("Desaturate", &isDesaturate))
			{
				mParticleSystem->SetDesaturate(isDesaturate);
			}

			if (isDesaturate)
			{
				/* Desaturate Power */
				{
					float desaturatePower = mParticleSystem->GetDesaturatePower();
					if (ImGui::DragFloat("Desaturate Power", &desaturatePower, 0.01f, 0, FLT_MAX))
					{
						mParticleSystem->SetDesaturatePower(desaturatePower);
					}
				}
			}
		}

		auto colorTex = mParticleSystem->GetColorTexture();
		if (colorTex)
		{
			if (ImGui::TreeNodeEx("Color Texture", TREE_NODE_FLAGS))
			{
				colorTex->RenderImgui(128, 128);
				ImGui::TreePop();
			}
		}

		auto alphaTex = mParticleSystem->GetAlphaTexture();
		if (alphaTex)
		{
			if (ImGui::TreeNodeEx("Alpha Texture", TREE_NODE_FLAGS))
			{
				alphaTex->RenderImgui(128, 128);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}