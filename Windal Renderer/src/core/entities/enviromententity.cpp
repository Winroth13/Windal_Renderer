#include "core/entities/enviromententity.h"
#include "core/scene.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

EnviromentEntity::EnviromentEntity(const std::string& name)
	:Entity(name)
{
}

EnviromentEntity::~EnviromentEntity()
{
}

void EnviromentEntity::UpdateSelf(double delta)
{
}

void EnviromentEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.UpdateEnviroment(GetScene().GetEnviroment());
}

void EnviromentEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Enviroment", TREE_NODE_FLAGS))
	{
		auto& enviroment = GetScene().GetEnviroment();
		ImGui::ColorEdit3("Ambient Color", &enviroment.GetAmbientColorRef().x);
		ImGui::ColorEdit3("Sun Color", &enviroment.GetSunColorRef().x);
		ImGui::DragFloat3("Sun Direction", &enviroment.GetSunDirectionRef().x, 0.02f);
		ImGui::TreePop();
	}
}