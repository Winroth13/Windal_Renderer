#include "core/renderer/particlerenderer.h"

#include "graphics/shaders/computeshader.h"
#include "graphics/shaders/geometryshader.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/textures/texture2d.h"
#include "core/renderer/renderserver.h"

#include "core/renderer/renderer.h"

#include "math/transform.h"

#include <DirectXMath.h>

ParticleRenderer::ParticleRenderer()
{
}

ParticleRenderer::~ParticleRenderer()
{
}

bool ParticleRenderer::Create()
{
	mParticleVertexShader = std::make_unique<VertexShader>("resources/ParticleVertexShader.cso");
	mParticlePixelShader = std::make_unique<PixelShader>("resources/ParticlePixelShader.cso");
	mBillboardGeometryShader = std::make_unique<GeometryShader>("resources/BillboardGeometryShader.cso");

	return true;
}

void ParticleRenderer::Render(
	ID3D11DeviceContext* ctx,
	RenderServer& renderServer,
	std::vector<ParticleSystemData>& data
)
{
	constexpr UINT PARTICLE_COLOR_TEXTURE_SLOT = 0;
	constexpr UINT PARTICLE_ALPHA_TEXTURE_SLOT = 1;

	if (data.size() < 1)
	{
		return;
	}

	/* Configure Input Assembler */
	ctx->IASetInputLayout(nullptr);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* Bind Shaders */
	ctx->VSSetShader(mParticleVertexShader->GetShader(), nullptr, 0);
	ctx->PSSetShader(mParticlePixelShader->GetShader(), nullptr, 0);
	ctx->GSSetShader(mBillboardGeometryShader->GetShader(), nullptr, 0);

	float blendFactor[4] = { 0.8f, 0.8f, 0.8f, 0.0f };
	UINT sampleMask = 0xffffffff;

	D3D11_BLEND_DESC desc = {};
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* blendState = nullptr;
	Renderer::GetDevice()->CreateBlendState(&desc, &blendState);

	ctx->OMSetBlendState(blendState, blendFactor, sampleMask);
	blendState->Release();

	for (ParticleSystemData& p : data)
	{
		DirectX::XMMATRIX worldMatrix = p.transform.GetMatrix();
		renderServer.UpdatePerObject(worldMatrix);

		/* Bind Particle System Buffer */
		ID3D11Buffer* systemBuffer = p.system->GetParticleSystemBuffer();
		ctx->GSSetConstantBuffers(0, 1, &systemBuffer);
		ctx->PSSetConstantBuffers(0, 1, &systemBuffer);

		/* Bind Textures */
		ID3D11ShaderResourceView* colorTexView = p.system->GetColorTexture()->GetSRV();
		ID3D11ShaderResourceView* alphaTexView = p.system->GetAlphaTexture()->GetSRV();
		ctx->PSSetShaderResources(PARTICLE_COLOR_TEXTURE_SLOT, 1, &colorTexView);
		ctx->PSSetShaderResources(PARTICLE_ALPHA_TEXTURE_SLOT, 1, &alphaTexView);

		ID3D11ShaderResourceView* srv = p.system->GetSRV();
		ctx->VSSetShaderResources(0, 1, &srv);

		ctx->Draw(static_cast<UINT>(p.system->GetCount()), 0);
	}

	/* Unbind Particle System Buffer */
	ID3D11Buffer* nullBuffer = nullptr;
	ctx->GSSetConstantBuffers(0, 1, &nullBuffer);
	ctx->PSSetConstantBuffers(0, 1, &nullBuffer);

	/* Unbind Textures */
	ID3D11ShaderResourceView* nullSrv = nullptr;
	ctx->PSSetShaderResources(PARTICLE_COLOR_TEXTURE_SLOT, 1, &nullSrv);
	ctx->PSSetShaderResources(PARTICLE_ALPHA_TEXTURE_SLOT, 1, &nullSrv);

	/* Unbind SRV */
	ID3D11ShaderResourceView* srvNull = nullptr;
	ctx->VSSetShaderResources(0, 1, &srvNull);

	/* Unbind Shaders */
	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);

	ctx->OMSetBlendState(nullptr, blendFactor, sampleMask);
}