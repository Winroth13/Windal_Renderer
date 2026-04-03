#pragma once
#include <iostream>
#include <vector>
#include <DirectXMath.h>

class Entity
{
public:
	Entity() : mName("Entity") {};
	Entity(const std::string& name);
	virtual ~Entity() = default;

	void Update(float delta);
	void Render();
	void RenderImgui();

	inline std::string& GetName() { return mName; }
	void SetName(const std::string& name) { mName = name; }

	inline size_t GetChildCount() const { return mChildren.size(); }
	inline std::vector<std::unique_ptr<Entity>>& GetChildren() { return mChildren; }

	inline bool IsVisible() { return mVisible; }

	inline DirectX::XMFLOAT3& GetPosition() { return mPosition; }

	inline DirectX::XMFLOAT3& GetGlobalPosition()
	{
		if (mAttach)
		{
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&GetPosition());
			DirectX::XMVECTOR attachPosition = DirectX::XMLoadFloat3(&mAttach->GetPosition());
			DirectX::XMFLOAT3 globalPosition;
			DirectX::XMStoreFloat3(&globalPosition, DirectX::XMVectorAdd(position, attachPosition));
			return globalPosition;
		}
		else
		{
			return GetPosition();
		}
	}

	inline DirectX::XMMATRIX GetGlobalTransform()
	{
		DirectX::XMMATRIX transformMatrix = DirectX::XMMatrixIdentity();
		transformMatrix *= DirectX::XMMatrixRotationRollPitchYaw(mAngles.x, mAngles.y, mAngles.z);
		transformMatrix *= DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		transformMatrix *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

		if (mAttach)
		{
			transformMatrix *= mAttach->GetGlobalTransform();
		}

		return transformMatrix;
	}

	void SetPosition(const DirectX::XMFLOAT3& position) { mPosition = position; }
	void SetPosition(const float x, const float y, const float z) { mPosition = DirectX::XMFLOAT3(x, y, z); }

	inline DirectX::XMFLOAT3& GetAngles() { return mAngles; }
	void SetAngles(const DirectX::XMFLOAT3& angles) { mAngles = angles; }
	void SetAngles(const float pitch, const float yaw, const float roll) { mAngles = DirectX::XMFLOAT3(pitch, yaw, roll); }

	inline DirectX::XMFLOAT3& GetScale() { return mScale; }

	void SetScale(const DirectX::XMFLOAT3& scale) { mScale = scale; }
	void SetScale(const float x, const float y, const float z) { mScale = DirectX::XMFLOAT3(x, y, z); }

	void Attach(Entity* entity) { mAttach = entity; }

protected:
	virtual void UpdateSelf(float delta) {};
	virtual void RenderSelf() {};
	virtual void RenderImguiSelf() {};

	std::string mName;
	bool mVisible = true;
	DirectX::XMFLOAT3 mPosition = { 0,0,0 };
	DirectX::XMFLOAT3 mAngles = { 0,0,0 };
	DirectX::XMFLOAT3 mScale = { 1, 1, 1 };

	Entity* mAttach = nullptr;
};