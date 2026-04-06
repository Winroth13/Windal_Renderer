#include "core/entity.h"
#include "imgui/imgui.h"

Entity::Entity(const std::string& name)
{
	mName = name;
}

void Entity::Update(float delta)
{
	UpdateSelf(delta);
}

void Entity::Render()
{
	if (IsVisible())
	{
		RenderSelf();
	}
}

void Entity::RenderImgui()
{
	if (ImGui::TreeNodeEx(mName.c_str(), ImGuiTreeNodeFlags_DrawLinesToNodes))
	{
		ImGui::Text("Transform");
		DirectX::XMFLOAT3 position = transform.GetPosition3f();
		DirectX::XMFLOAT3 angles = transform.GetAngles3f();
		DirectX::XMFLOAT3 scale = transform.GetScale3f();

		if (ImGui::DragFloat3("Position", &position.x, 0.1f))
		{
			transform.SetPosition(position);
		}

		if (ImGui::DragFloat3("Angles", &angles.x, 0.1f))
		{
			transform.SetAngles(angles);
		}

		if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
		{
			transform.SetScale(scale);
		}

		if (mAttach)
		{
			ImGui::Text("Attached to: %s", mAttach->GetName().c_str());
		}
		else
		{
			ImGui::Text("Attached to: None");
		}
		
		RenderImguiSelf();

		ImGui::TreePop();
	}
}

bool Entity::IsVisible()
{
	if (mAttach && !mAttach->IsVisible())
	{
		return false;
	}
	return mVisible;
}

DirectX::XMMATRIX Entity::GetGlobalTransform()
{
	DirectX::XMMATRIX mat = transform.GetMatrix();
	if (mAttach != nullptr)
	{
		mat *= mAttach->GetGlobalTransform();
	}
	return mat;
}
