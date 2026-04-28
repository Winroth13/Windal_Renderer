#include "core/renderer/aabbrenderer.h"
#include "core/renderer/renderer.h"
#include "core/logger.h"

#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"

#include <d3d11.h>

constexpr int NUM_VERTS = 8;
constexpr int NUM_INDICIES = 24;

AABBRenderer::AABBRenderer()
{
}

AABBRenderer::~AABBRenderer()
{
	if (mInputLayout)
	{
		mInputLayout->Release();
	}

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
	}

	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
	}
}

bool AABBRenderer::Create()
{
	mVertexShader = std::make_unique<VertexShader>("resources/AABBVertexShader.cso");
	mPixelShader = std::make_unique<PixelShader>("resources/AABBPixelShader.cso");

	D3D11_INPUT_ELEMENT_DESC inputDesc[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr = Renderer::GetDevice()->CreateInputLayout(
		inputDesc,
		2,
		mVertexShader->GetByteCode().c_str(),
		mVertexShader->GetByteCode().length(),
		&mInputLayout
	);

	if (FAILED(hr))
	{
		Logger::Error("Failed to create AABB input layout");
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = UINT(sizeof(AABBVertex) * NUM_VERTS);
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = Renderer::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &mVertexBuffer);

	if (FAILED(hr))
	{
		Logger::Error("Failed to create AABB vertex buffer");
		return false;
	}

	D3D11_BUFFER_DESC indexbufferDesc;
	indexbufferDesc.ByteWidth = UINT(sizeof(uint32_t) * NUM_INDICIES);
	indexbufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.MiscFlags = 0;

	constexpr uint32_t indices[] = {
		// Back Side
		0, 2,  0, 4,  2, 6,  4, 6,
		// Front Side
		1, 3,  1, 5,  3, 7,  5, 7,
		// Right Side
		0, 1,  2, 3,
		// Left Side
		4, 5,  6, 7
	};

	D3D11_SUBRESOURCE_DATA indexbufferData;
	indexbufferData.pSysMem = indices;
	indexbufferData.SysMemPitch = 0;
	indexbufferData.SysMemSlicePitch = 0;

	hr = Renderer::GetDevice()->CreateBuffer(&indexbufferDesc, &indexbufferData, &mIndexBuffer);

	if (FAILED(hr))
	{
		Logger::Error("Failed to create AABB index buffer");
		return false;
	}

	return true;
}

void AABBRenderer::Render(ID3D11DeviceContext* ctx, const std::vector<AABBData>& data)
{
	if (data.size() == 0)
	{
		return;
	}

	/* Setup Input Assembler */
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ctx->IASetInputLayout(mInputLayout);

	/* Bind Shaders	*/
	ctx->VSSetShader(mVertexShader->GetShader(), nullptr, 0);
	ctx->PSSetShader(mPixelShader->GetShader(), nullptr, 0);

	for (auto& aabb : data)
	{
		AABBVertex vertices[NUM_VERTS];

		DirectX::XMFLOAT3 corners[NUM_VERTS] = {};

		for (int i = 0; i < 4; ++i)
		{
			if (i < 2)
			{
				corners[i * 2].x = aabb.origin.x + aabb.size.x / 2;
				corners[i * 2 + 1].x = aabb.origin.x + aabb.size.x / 2;
			}
			else
			{
				corners[i * 2].x = aabb.origin.x - aabb.size.x / 2;
				corners[i * 2 + 1].x = aabb.origin.x - aabb.size.x / 2;
			}

			if (i < 2)
			{
				corners[i].y = aabb.origin.y + aabb.size.y / 2;
				corners[i + 4].y = aabb.origin.y + aabb.size.y / 2;
			}
			else
			{
				corners[i].y = aabb.origin.y - aabb.size.y / 2;
				corners[i + 4].y = aabb.origin.y - aabb.size.y / 2;
			}

			if (i % 2 == 0)
			{
				corners[i].z = aabb.origin.z + aabb.size.z / 2;
				corners[i + 4].z = aabb.origin.z + aabb.size.z / 2;
			}
			else
			{
				corners[i].z = aabb.origin.z - aabb.size.z / 2;
				corners[i + 4].z = aabb.origin.z - aabb.size.z / 2;
			}
		}

		for (int i = 0; i < NUM_VERTS; ++i)
		{
			AABBVertex vertex = {};
			vertex.mColor = aabb.color;
			vertex.mPosition = corners[i];
			vertices[i] = vertex;
		}

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		ctx->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, vertices, sizeof(AABBVertex) * NUM_VERTS);
		ctx->Unmap(mVertexBuffer, 0);

		UINT stride = sizeof(AABBVertex);
		UINT offset = 0;
		ctx->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		ctx->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		ctx->DrawIndexed(NUM_INDICIES, 0, 0);
	}

	/* Unbind shaders */
	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);

	/* Reset topology */
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}