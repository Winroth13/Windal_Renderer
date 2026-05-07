#pragma once
#include <DirectXMath.h>
#include <memory>

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

class ComputeShader;
class Texture2D;

enum class ParticleSystemFlags : uint32_t
{
	NONE = 0,
	ANIMATED = 1,
	DESATURATE = 2,
	ADDITIVE = 4,
	RESET = 8
};

struct ParticleSystemBuffer
{
	uint32_t ticks;
	float deltaTime;
	float lifetime = 5.0f;
	float spawnRadius = 0.1f;
	float velocity = 1.0f;
	uint32_t flags = static_cast<uint32_t>(ParticleSystemFlags::RESET);
	float atlasWidth = 1.0f;
	float atlasHeight = 1.0f;
	float atlasSpeed = 1.0f;
	float desaturatePow = 1.0f;
	float startScale = 1.0f;
	float endScale = 1.0f;
};

struct ParticleBuffer
{
	DirectX::XMFLOAT3 position = { 0,0,0 };
	float scale = 1.0f;
	DirectX::XMFLOAT3 tint = { 1,1,1 };
	float lifetime = 0;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t count);
	~ParticleSystem();

	void Update(double deltaTime);

	ID3D11Buffer* GetParticleSystemBuffer() { return mParticleSystemBuffer; }
	ID3D11ShaderResourceView* GetSRV() { return mSRV; }
	uint32_t GetCount() const { return mParticleCount; }

	float GetSpawnRadius() const { return mParticleSystemConstants.spawnRadius; }
	float GetVelocity() const { return mParticleSystemConstants.velocity; }
	float GetLifeTime() const { return mParticleSystemConstants.lifetime; }
	float GetAtlasWidth() const { return mParticleSystemConstants.atlasWidth; }
	float GetAtlasHeight() const { return mParticleSystemConstants.atlasHeight; }
	float GetAtlasSpeed() const { return mParticleSystemConstants.atlasSpeed; }
	float GetDesaturatePower() const { return mParticleSystemConstants.desaturatePow; }
	float GetStartScale() const { return mParticleSystemConstants.startScale; }
	float GetEndScale() const { return mParticleSystemConstants.endScale; }

	bool IsAnimated();
	bool IsDesaturate();
	bool IsAdditive();

	std::shared_ptr<Texture2D> GetColorTexture() { return mColorTexture; }
	std::shared_ptr<Texture2D> GetAlphaTexture() { return mAlphaTexture; }

	void SetCount(uint32_t count);

	void SetSpawnRadius(float radius) { mParticleSystemConstants.spawnRadius = radius; }
	void SetVelocity(float velocity) { mParticleSystemConstants.velocity = velocity; }
	void SetLifeTime(float value) { mParticleSystemConstants.lifetime = value; }
	void SetAtlasWidth(float width) { mParticleSystemConstants.atlasWidth = width; }
	void SetAtlasHeight(float height) { mParticleSystemConstants.atlasHeight = height; }
	void SetAtlasSpeed(float speed) { mParticleSystemConstants.atlasSpeed = speed; }
	void SetDesaturatePower(float value) { mParticleSystemConstants.desaturatePow = value; }
	void SetStartScale(float value) { mParticleSystemConstants.startScale = value; }
	void SetEndScale(float value) { mParticleSystemConstants.endScale = value; }

	void SetColorTexture(std::shared_ptr<Texture2D> colorTexture) { mColorTexture = colorTexture; }
	void SetAlphaTexture(std::shared_ptr<Texture2D> alphaTexture) { mAlphaTexture = alphaTexture; }

	void SetDesaturate(bool enabled);
	void SetAnimated(bool enabled);
	void SetAdditive(bool enabled);

	void Reset();

private:
	void CreateParticleBuffer(uint32_t count);

	ParticleSystemBuffer mParticleSystemConstants;

	uint32_t mParticleCount = 0;

	ID3D11Buffer* mParticleSystemBuffer = nullptr;
	ID3D11Buffer* mParticleBuffer = nullptr;
	ID3D11ShaderResourceView* mSRV = nullptr;
	ID3D11UnorderedAccessView* mUAV = nullptr;

	std::unique_ptr<ComputeShader> mComputeShader = nullptr;
	std::shared_ptr<Texture2D> mColorTexture = nullptr;
	std::shared_ptr<Texture2D> mAlphaTexture = nullptr;
};