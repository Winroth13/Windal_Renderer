#include "core/renderer/particlerenderer.h"

#include "graphics/shaders/computeshader.h"
#include "graphics/shaders/geometryshader.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"

#include "core/renderer/renderserver.h"

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

	for (ParticleSystemData& p : data)
	{
		DirectX::XMMATRIX worldMatrix = p.transform.GetMatrix();
		renderServer.UpdatePerObject(worldMatrix);

		/* Bind Particle System Buffer */
		ID3D11Buffer* systemBuffer = p.system->GetParticleSystemBuffer();
		ctx->GSSetConstantBuffers(0, 1, &systemBuffer);

		ID3D11ShaderResourceView* srv = p.system->GetSRV();
		ctx->VSSetShaderResources(0, 1, &srv);

		ctx->Draw(static_cast<UINT>(p.system->GetCount()), 0);
	}

	/* Unbind SRV */
	ID3D11ShaderResourceView* srvNull = nullptr;
	ctx->VSSetShaderResources(0, 1, &srvNull);

	/* Unbind Shaders */
	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
}