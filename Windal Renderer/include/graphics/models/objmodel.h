#pragma once
#include "graphics/models/model.h"
#include <string>

class VertexShader;
class PixelShader;

class OBJModel : public Model
{
public:
	OBJModel(
		const std::string& path,
		std::shared_ptr<VertexShader> vertexShader,
		std::shared_ptr<PixelShader> pixelShader,
		const bool isLeftHanded = false
	);

	~OBJModel();
};