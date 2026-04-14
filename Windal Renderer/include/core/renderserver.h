#pragma once
#include <memory>
#include <DirectXMath.h>

class Renderer;
class Model;
class Transform;
class Mesh;
class Material;
class Enviroment;

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

	void PushMesh(std::shared_ptr<Mesh> mesh, DirectX::XMMATRIX transform);
	void PushMaterial(std::shared_ptr<Material> material);

	void PushDirectionalLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color, float intensity);
	void PushPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float attenuation, float intensity);
	void PushSpotLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color, float angle, float intensity, float attenuation);

	void UpdateCamera(const DirectX::XMMATRIX viewProj, const DirectX::XMFLOAT3 cameraPos);

	void UpdateEnviroment(Enviroment& enviroment);

private:
	Renderer* mRenderer = nullptr;
};