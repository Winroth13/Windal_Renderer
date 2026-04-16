#pragma once
#include <vector>
#include <string>

#include "core/renderserver.h"

struct ID3D11Buffer;
struct D3D11_INPUT_ELEMENT_DESC;

struct Vertex
{
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mNormal;
	DirectX::XMFLOAT2 mUV;

	Vertex(
		const DirectX::XMFLOAT3 position,
		const DirectX::XMFLOAT3 normal,
		const DirectX::XMFLOAT2 uv
	) : mPosition(position), mNormal(normal), mUV(uv)
	{
	}
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool Create(
		const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indicies
	);

	void RenderImgui();

	const size_t GetNumIndicies() { return mNumIndicies; }
	const size_t GetNumVertices() { return mNumVertices; }

	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }

	void SetName(const std::string& name) { mName = name; }
	const std::string& GetName() { return mName; }

private:
	size_t mNumVertices = 0;
	size_t mNumIndicies = 0;

	std::string mName;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
};