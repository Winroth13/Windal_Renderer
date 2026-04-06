#pragma once
#include "graphics/textures/texture2d.h"
#include <string>

class ImageTexture2D : public Texture2D
{
public:
	ImageTexture2D(const std::string& path);
	~ImageTexture2D() override;

	uint32_t GetWidth() override { return mWidth; }
	uint32_t GetHeight() override { return mHeight; }
	inline const std::string& GetPath() const { return mPath; }

private:
	std::string mPath;
	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	uint32_t mChannels = 0;
};