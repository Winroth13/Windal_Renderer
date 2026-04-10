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
#include "core/entities/cameraentity.h"
#include "core/entities/enviromententity.h"

#include <memory>

class TestApp : public App
{
public:
	std::unique_ptr<Entity>* inspectorEntity = nullptr;
	Entity* cameraEntity;
	POINT previousMousePos;

	void Initialize() override
	{
		auto vShader = std::make_shared<VertexShader>("resources/VertexShader.cso");
		auto pShader = std::make_shared<PixelShader>("resources/PixelShader.cso");
		auto model = std::make_shared<OBJModel>("assets/capsule/capsule.obj", vShader, pShader);

		auto& entity = mScene->CreateEntity<ModelEntity>("Model 1", model);
		entity.transform.SetPosition(0, 0, 3);

		cameraEntity = &mScene->CreateEntity<CameraEntity>("Camera");

		auto& enviromentEntity = mScene->CreateEntity<EnviromentEntity>("Enviroment");
	};

	void Shutdown() override
	{
	};

	void Update(double delta) override
	{
		/* Camera Movement	*/
		constexpr float SPEED = 3.0f;
		constexpr float TURN_SPEED = 2.5f;

		POINT newMousePos;

		GetCursorPos(&newMousePos);

		/* Movement */
		if (GetAsyncKeyState('W') & 0x8000)
			cameraEntity->transform.MoveForward(SPEED * (float)delta);
		if (GetAsyncKeyState('S') & 0x8000)
			cameraEntity->transform.MoveForward(-SPEED * (float)delta);
		if (GetAsyncKeyState('A') & 0x8000)
			cameraEntity->transform.MoveRight(-SPEED * (float)delta);
		if (GetAsyncKeyState('D') & 0x8000)
			cameraEntity->transform.MoveRight(SPEED * (float)delta);
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			cameraEntity->transform.MoveUp(SPEED * (float)delta);
		if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
			cameraEntity->transform.MoveUp(-SPEED * (float)delta);

		/* Rotation */
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			cameraEntity->transform.RotateY(-TURN_SPEED * (float)delta);
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			cameraEntity->transform.RotateY(TURN_SPEED * (float)delta);
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			cameraEntity->transform.RotateX(-TURN_SPEED * (float)delta);
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			cameraEntity->transform.RotateX(TURN_SPEED * (float)delta);
		if (GetAsyncKeyState(MK_RBUTTON) & 0x8000)
		{
			float dx = 4 * DirectX::XMConvertToRadians(static_cast<float>(newMousePos.x - previousMousePos.x));
			float dy = 4 * DirectX::XMConvertToRadians(static_cast<float>(newMousePos.y - previousMousePos.y));

			cameraEntity->transform.RotateX(TURN_SPEED * dy * (float)delta);
			cameraEntity->transform.RotateY(TURN_SPEED * dx * (float)delta);
		}

		previousMousePos = newMousePos;
	};

	void Render(RenderServer& renderServer) override
	{
	};

	void ImguiRender() override
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					Quit();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::Checkbox("Scene Hierarchy", &mShowHierarchy);
				ImGui::Checkbox("Inspector", &mShowInspector);
				ImGui::Checkbox("Diagnostics", &mShowDiagnostics);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (mShowHierarchy)
		{
			ImGui::Begin("Scene Hierarchy");

			if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& entity : mScene->GetEntities())
				{
					ImGuiTreeNodeFlags flags =
						ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

					if (inspectorEntity != nullptr && entity == *inspectorEntity)
					{
						flags |= ImGuiTreeNodeFlags_Selected;
					}

					ImGui::TreeNodeEx(entity->GetName().c_str(), flags);

					if (ImGui::IsItemClicked())
					{
						inspectorEntity = &entity;
					}
				}
				ImGui::TreePop();
			}

			ImGui::End();
		}

		if (mShowInspector)
		{
			ImGui::Begin("Inspector");
			if (inspectorEntity != nullptr)
			{
				inspectorEntity->get()->RenderImgui();
			}
			ImGui::End();
		}

		if (mShowDiagnostics)
		{
			ImGui::Begin("Diagnostics");
			ImGui::Text("%.2f FPS (%.1f ms)", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime * 1000);
			ImGui::Text("Resolution: %.0fx%.0f", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
			ImGui::End();
		}
	};

private:
	bool mShowHierarchy = true;
	bool mShowInspector = true;
	bool mShowDiagnostics = true;
};

App* CreateApp()
{
	return new TestApp();
}

WindowProps CreateWindowProperties()
{
	WindowProps props;
	props.title = "Windal Renderer";
	props.width = 1280;
	props.height = 640;

	return props;
}