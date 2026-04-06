#include "main.h"
#include "imgui/imgui.h"

class TestApp : public App
{
public:
	void Initialize() override
	{
		auto& entity1 = mScene->CreateEntity<Entity>("Hello World!");
		auto& entity2 = mScene->CreateEntity<Entity>("Hello World 2!");

		entity2.Attach(&entity1);
	};

	void Shutdown() override
	{
	};

	void Update(float delta) override
	{
	};

	void Render() override
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

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx("Scene", flags))
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