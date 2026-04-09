#pragma once
#include <memory>
#include <DirectXMath.h>

class Renderer;
class Model;
class Transform;

struct ID3D11DeviceContext;
enum ConstantBufferType
{
	BUFFER_PER_FRAME = 0,
	BUFFER_PER_VIEW = 1,
	BUFFER_PER_OBJECT = 2,
	BUFFER_PER_MATERIAL = 3
};

class RenderServer
{
public:
	RenderServer() {}
	~RenderServer() {}

	bool Create(Renderer* renderer);
	ID3D11DeviceContext* GetContext();

	void RenderModel(std::shared_ptr<Model> model, Transform& transform);
	void UpdateCamera(const DirectX::XMMATRIX viewProj, const DirectX::XMFLOAT3 cameraPos);

private:
	Renderer* mRenderer = nullptr;
};