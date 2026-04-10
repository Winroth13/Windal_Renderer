#pragma once
#include <array>
#include <vector>

#include "core/renderserver.h"

struct ID3D11Buffer;
struct D3D11_INPUT_ELEMENT_DESC;

struct Vertex
{
	std::array<float, 3> mPosition;
	std::array<float, 3> mNormal;
	std::array<float, 2> mUV;

	Vertex(
		const std::array<float, 3> position,
		const std::array<float, 3> normal,
		const std::array<float, 2> uv
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

	size_t GetNumIndicies() { return mNumIndicies; }

	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }

private:
	size_t mNumVertices = 0;
	size_t mNumIndicies = 0;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
};