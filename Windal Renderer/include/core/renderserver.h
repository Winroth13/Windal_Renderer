#pragma once

class Renderer;
struct ID3D11DeviceContext;

class RenderServer
{
public:
	RenderServer() {}
	~RenderServer() {}
	
	bool Create(Renderer* renderer);
	ID3D11DeviceContext* GetContext();

private:
	Renderer* mRenderer = nullptr;
};