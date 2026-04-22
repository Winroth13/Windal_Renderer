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
#include "core/entities/pointlightentity.h"
#include "core/entities/directionallightentity.h"
#include "core/entities/spotlightentity.h"

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
		auto sponza = std::make_shared<OBJModel>("assets/sponza/sponza.obj", vShader, true);

		auto& entity = mScene->CreateEntity<ModelEntity>("Sponza", sponza);
		entity.transform.SetScale(0.01f, 0.01f, 0.01f);
		entity.transform.SetAngles(0, DirectX::XM_PI / 2, 0);

		cameraEntity = &mScene->CreateEntity<CameraEntity>("Camera");
		cameraEntity->transform.SetPosition(0, 1, 0);

		auto& enviromentEntity = mScene->CreateEntity<EnviromentEntity>("Enviroment");
		enviromentEntity.SetAmbientColor(108.f / 255, 150.f / 255, 177.f / 255);

		auto& pointLightEntity1 = mScene->CreateEntity<PointLightEntity>("Point Light 1");
		pointLightEntity1.SetColor({ 0, 1, 0 });
		pointLightEntity1.SetVisble(false);

		auto& sunEntity = mScene->CreateEntity<DirectionalLightEntity>("Sun");
		sunEntity.transform.SetAngles(DirectX::XMConvertToRadians(60), DirectX::XMConvertToRadians(-100), 0);
		sunEntity.SetIntensity(0.75f);
		sunEntity.SetVisble(true);

		auto& spotEntity = mScene->CreateEntity<SpotLightEntity>("Spot Light 1");
		spotEntity.SetColor({ 1.0, 0.0, 0.0 });
		spotEntity.transform.SetPosition(0, 2, 0);
		spotEntity.transform.SetAngles(0, (float)3.14 / 2, 0);
		spotEntity.SetIntensity(32);
		spotEntity.SetVisble(false);

		auto& spotEntity2 = mScene->CreateEntity<SpotLightEntity>("Spot Light 2");
		spotEntity2.SetColor({ 0.0, 1.0, 0.0 });
		spotEntity2.transform.SetPosition(0, 2, 0);
		spotEntity2.transform.SetAngles(0, 0, 0);
		spotEntity2.SetIntensity(32);
		spotEntity2.SetVisble(false);
	};

	void Shutdown() override
	{
	};

	void Update(double delta) override
	{
		/* Only handle input if window is focused */
		if (GetFocus() != NULL)
		{
			/* Camera Movement	*/
			constexpr float SPEED = 3.0f;
			constexpr float TURN_SPEED = 2.6f;

			POINT newMousePos;

			GetCursorPos(&newMousePos);

			float speed = SPEED;
			if (GetAsyncKeyState(VK_LSHIFT))
			{
				speed *= 3.0f;
			}

			/* Movement */
			if (GetAsyncKeyState('W') & 0x8000)
				cameraEntity->transform.MoveForward(speed * (float)delta);
			if (GetAsyncKeyState('S') & 0x8000)
				cameraEntity->transform.MoveForward(-speed * (float)delta);
			if (GetAsyncKeyState('A') & 0x8000)
				cameraEntity->transform.MoveRight(-speed * (float)delta);
			if (GetAsyncKeyState('D') & 0x8000)
				cameraEntity->transform.MoveRight(speed * (float)delta);
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				cameraEntity->transform.MoveUp(speed * (float)delta);
			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
				cameraEntity->transform.MoveUp(-speed * (float)delta);

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
				float dx = 20 * DirectX::XMConvertToRadians(static_cast<float>(newMousePos.x - previousMousePos.x));
				float dy = 20 * DirectX::XMConvertToRadians(static_cast<float>(newMousePos.y - previousMousePos.y));

				cameraEntity->transform.RotateX(TURN_SPEED * dy * (float)delta);
				cameraEntity->transform.RotateY(TURN_SPEED * dx * (float)delta);
			}

			previousMousePos = newMousePos;
		}
	};

	void Render(RenderServer& renderServer) override
	{
	};

	void ImguiRender(RenderServer& renderServer) override
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

			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::BeginMenu("Viewport"))
				{
					bool changed = false;

					if (ImGui::RadioButton("Default", mViewportDebugMode == 0))
					{
						mViewportDebugMode = 0;
						changed = true;
					}

					if (ImGui::RadioButton("Wireframe", mViewportDebugMode == 1))
					{
						mViewportDebugMode = 1;
						changed = true;
					}

					if (ImGui::RadioButton("GBuffers", mViewportDebugMode == 2))
					{
						mViewportDebugMode = 2;
						changed = true;
					}

					if (changed)
					{
						renderServer.SetWireframe(mViewportDebugMode == 1);
						renderServer.SetShowGBuffer(mViewportDebugMode == 2);
					}

					ImGui::EndMenu();
				}

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
	uint32_t mViewportDebugMode = 0;

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