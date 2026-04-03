#include "main.h"

#include "imgui/imgui.h"

class TestApp : public App
{
public:
	void Initialize() override
	{
	};

	void Shutdown() override
	{
	};

	void Update() override
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
				bool temp = false;
				ImGui::Checkbox("Scene Hierarchy", &temp);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::ShowMetricsWindow();
	};
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