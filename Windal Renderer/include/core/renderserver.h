#pragma once

class Renderer;
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
	void RenderIndexed(size_t numIndicies);

private:
	Renderer* mRenderer = nullptr;
};