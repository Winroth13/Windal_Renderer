#pragma once
#include <DirectXMath.h>
#include <memory>

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

class ComputeShader;

struct ParticleSystemBuffer
{
	float deltaTime;
	float lifetime;
	float atlasWidth;
	float atlasHeight;
};

struct ParticleBuffer
{
	DirectX::XMFLOAT3 position = { 0,0,0 };
	float scale = 1.0f;
	DirectX::XMFLOAT3 tint = { 1,1,1 };
	float lifetime = 5.0f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t count);
	~ParticleSystem();

	void Update();

	ID3D11Buffer* GetParticleSystemBuffer() { return mParticleSystemBuffer; }
	ID3D11ShaderResourceView* GetSRV() { return mSRV; }
	uint32_t GetCount() const { return mParticleCount; }

	void SetLifeTime(float value) { mLifetime = value; }
	void SetAtlasWidth(float width) { mAtlasWidth = width; }
	void SetAtlasHeight(float height) { mAtlasHeight = height; }

private:
	float mLifetime = 5.0f;
	float mAtlasWidth = 1.0f;
	float mAtlasHeight = 1.0f;

	uint32_t mParticleCount = 0;

	ID3D11Buffer* mParticleSystemBuffer = nullptr;
	ID3D11Buffer* mParticleBuffer = nullptr;
	ID3D11ShaderResourceView* mSRV = nullptr;
	ID3D11UnorderedAccessView* mUAV = nullptr;

	std::unique_ptr<ComputeShader> mComputeShader;
};