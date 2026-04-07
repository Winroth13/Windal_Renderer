#include "main.h"
#include "core/imguiflags.h"
#include "imgui/imgui.h"

#include "core/logger.h"

#include "graphics/textures/imagetexture2d.h"
#include "graphics/models/objmodel.h"

#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/materials/material.h"

#include "core/entities/modelentity.h"

#include <memory>

class TestApp : public App
{
public:
	void Initialize() override
	{
		auto vShader = std::make_shared<VertexShader>("resources/VertexShader.cso");
		auto pShader = std::make_shared<PixelShader>("resources/PixelShader.cso");
		auto model = std::make_shared<OBJModel>("assets/capsule/capsule.obj", vShader, pShader);

		auto& entity = mScene->CreateEntity<ModelEntity>("Model 1", model);
		entity.transform.SetPosition(0, 0, 10);
	};

	void Shutdown() override
	{
	};

	void Update(float delta) override
	{
	};

	void Render(RenderServer& renderServer) override
	{
	};

	void ImguiRender() override
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::Checkbox("Scene Hierarchy", &mShowHierarchy);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (mShowHierarchy)
		{
			ImGui::Begin("Scene Hierarchy");

			if (ImGui::TreeNodeEx("Scene", TREE_NODE_FLAGS))
			{
				for (auto& entity : mScene->GetEntities())
				{
					entity->RenderImgui();
				}
				ImGui::TreePop();
			}

			ImGui::End();
		}
	};

private:
	bool mShowHierarchy = true;
};

App* CreateApp()
{
	return new TestApp();
}

WindowProps CreateWindowProperties()
{
	WindowProps props;
	props.title = "Vindal Renderer";
	props.width = 1280;
	props.height = 640;

	return props;
}