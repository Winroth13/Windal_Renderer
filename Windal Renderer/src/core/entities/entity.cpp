#include "core/entities/entity.h"
#include "imgui/imgui.h"
#include "core/imguiflags.h"

Entity::Entity(const std::string& name)
{
	mName = name;
}

void Entity::Update(double delta)
{
	UpdateSelf(delta);
}

void Entity::Render(RenderServer& renderServer)
{
	if (IsVisible())
	{
		RenderSelf(renderServer);
	}
}

void Entity::RenderImgui()
{
	if (ImGui::TreeNodeEx("Transform", TREE_NODE_FLAGS))
	{
		DirectX::XMFLOAT3 position = transform.GetPosition3f();
		DirectX::XMFLOAT3 scale = transform.GetScale3f();

		if (ImGui::DragFloat3("Position", &position.x, 0.1f))
		{
			transform.SetPosition(position);
		}

		DirectX::XMFLOAT3 anglesDeg = transform.GetAngles3f();
		anglesDeg.x *= (180.0f / DirectX::XM_PI);
		anglesDeg.y *= (180.0f / DirectX::XM_PI);
		anglesDeg.z *= (180.0f / DirectX::XM_PI);

		if (ImGui::DragFloat3("Angles", &anglesDeg.x, 1, 0, 0, "%.1f deg"))
		{
			DirectX::XMFLOAT3 angles = anglesDeg;
			angles.x *= (DirectX::XM_PI / 180.0f);
			angles.y *= (DirectX::XM_PI / 180.0f);
			angles.z *= (DirectX::XM_PI / 180.0f);
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
		ImGui::TreePop();
	}

	RenderImguiSelf();
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