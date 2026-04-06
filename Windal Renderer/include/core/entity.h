#pragma once
#include <iostream>
#include <vector>
#include <DirectXMath.h>

#include "core/transform.h"
#include "core/renderserver.h"

class Entity
{
public:
	Entity() : mName("Entity") {};
	Entity(const std::string& name);

	virtual ~Entity() = default;

	void Update(float delta);
	void Render(RenderServer& renderServer);
	void RenderImgui();

	std::string& GetName() { return mName; }
	void SetName(const std::string& name) { mName = name; }

	bool IsVisible();

	DirectX::XMMATRIX GetGlobalTransform();

	void Attach(Entity* entity) { mAttach = entity; }

public:
	Transform transform;

protected:
	virtual void UpdateSelf(float delta) {};
	virtual void RenderSelf(RenderServer& renderServer) {};
	virtual void RenderImguiSelf() {};

	std::string mName;
	bool mVisible = true;

	Entity* mAttach = nullptr;
};