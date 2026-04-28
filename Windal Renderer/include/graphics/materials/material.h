#pragma once
#include "core/renderserver.h"

#include "graphics/shaders/vertexshader.h"
#include "graphics/textures/texture2d.h"

#include <DirectXMath.h>
#include <d3d11.h>

enum class MaterialFlags
{
	HAS_NORMAL_MAP = 1
};

class Material
{
public:
	Material(
		std::shared_ptr<VertexShader> vertexShader,
		std::shared_ptr<Texture2D> texture2D
	);
	virtual ~Material();

	inline std::shared_ptr<VertexShader> GetVertexShader() { return mVertexShader; }
	inline ID3D11InputLayout* GetInputLayout() { return mInputLayout; }

	inline std::shared_ptr<Texture2D> GetTexture() { return mTexture; }
    inline std::shared_ptr<Texture2D> GetNormalMap() { return mNormalMap; }
	inline std::shared_ptr<CubemapTexture> GetCubemapTexture() { return mCubemapTexture; }
	
	void SetAmbientCoefficient(const float r, const float g, const float b);
	void SetDiffuseCoefficient(const float r, const float g, const float b);
	void SetSpecularCoefficient(const float r, const float g, const float b);
	void SetPhongExponent(const float phongExponent);
	void SetReflectiveness(const float reflectiveness);

	void SetCubemapTexture(std::shared_ptr<CubemapTexture> cubemapTexture) { mCubemapTexture = cubemapTexture; }
	void SetNormalMap(std::shared_ptr<Texture2D> normalMap);

	void RenderImgui();

	inline DirectX::XMVECTOR GetAmbientCoefficient() { return DirectX::XMLoadFloat3(&mAmbientCoefficient); }
	inline DirectX::XMVECTOR GetDiffuseCoefficient() { return DirectX::XMLoadFloat3(&mDiffuseCoefficient); }
	inline DirectX::XMVECTOR GetSpecularCoefficient() { return DirectX::XMLoadFloat3(&mSpecularCoefficient); }

	inline DirectX::XMFLOAT3& GetAmbientCoefficient3f() { return mAmbientCoefficient; }
	inline DirectX::XMFLOAT3& GetDiffuseCoefficient3f() { return mDiffuseCoefficient; }
	inline DirectX::XMFLOAT3& GetSpecularCoefficient3f() { return mSpecularCoefficient; }

	inline float& GetPhongExponent() { return mPhongExponent; }
	inline float& GetReflectiveness() { return mReflectiveness; }
	inline uint32_t GetFlags() { return mFlags; }

	void SetName(const std::string& name) { mName = name; }
	const std::string& GetName() { return mName; }

private:
	bool mIsDirty = true;
	float mPhongExponent = 8;
	float mReflectiveness = 0;

	std::string mName;

	DirectX::XMFLOAT3 mAmbientCoefficient = { 1, 1, 1 };
	DirectX::XMFLOAT3 mDiffuseCoefficient = { 1, 1, 1 };
	DirectX::XMFLOAT3 mSpecularCoefficient = { 1, 1, 1 };

	std::shared_ptr<VertexShader> mVertexShader = nullptr;
	std::shared_ptr<Texture2D> mTexture = nullptr;
	std::shared_ptr<CubemapTexture> mCubemapTexture = nullptr;
	std::shared_ptr<Texture2D> mNormalMap = nullptr;

	uint32_t mFlags = 0;

	ID3D11InputLayout* mInputLayout = nullptr;
};