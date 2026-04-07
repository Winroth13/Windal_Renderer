#define NOMINMAX

#include "graphics/models/objmodel.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/meshes/mesh.h"
#include "graphics/textures/imagetexture2d.h"
#include "graphics/materials/material.h"

#include "core/logger.h"

#include <filesystem>

#include <DirectXMesh.h>
#include <WaveFrontReader.h>

OBJModel::OBJModel(
	const std::string& path,
	std::shared_ptr<VertexShader> vertexShader,
	std::shared_ptr<PixelShader> pixelShader,
	const bool isLeftHanded
)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indicies;

	DX::WaveFrontReader<uint32_t> reader;

	std::wstring wPath(path.begin(), path.end());
	HRESULT hr = reader.Load(wPath.c_str(), true, true);

	if (FAILED(hr))
	{
		Logger::Error("Failed to load OBJ from path: " + path);
		throw std::runtime_error("");
	}

	/* Read Vertices */
	vertices.reserve(reader.vertices.size());
	for (auto& vertex : reader.vertices)
	{
		if (isLeftHanded)
		{
			vertex.textureCoordinate.y = 1 - vertex.textureCoordinate.y;
		}

		Vertex resultVertex = Vertex
		(
			{ vertex.position.x, vertex.position.y, vertex.position.z },
			{ vertex.normal.x, vertex.normal.y, vertex.normal.z },
			{ vertex.textureCoordinate.x, vertex.textureCoordinate.y }
		);

		vertices.push_back(resultVertex);
	}

	indicies = reader.indices;

	if (mesh->Create(vertices, indicies) == false)
	{
		Logger::Error("OBJ Model failed to create mesh");
		throw std::runtime_error("");
	}

	SetMesh(mesh);

	// Gets material properties
	DX::WaveFrontReader<uint32_t>::Material& OBJmaterial = reader.materials[1];
	std::wstring wMtlFile(OBJmaterial.strTexture);

	// Calculate length of name
	int size = WideCharToMultiByte(
		CP_UTF8,                 // UTF-8 output
		0,
		wMtlFile.c_str(),
		static_cast<int>(wMtlFile.size()),
		nullptr,
		0,
		nullptr,
		nullptr
	);

	std::string textureFile = std::string(size, 0);

	// Convert from wstring to std::string
	WideCharToMultiByte(
		CP_UTF8,
		0,
		wMtlFile.c_str(),
		static_cast<int>(wMtlFile.size()),
		&textureFile[0],
		size,
		nullptr,
		nullptr
	);

	std::string dir = std::filesystem::path(path)
		.parent_path()
		.string();

	std::string texturePath = dir + "/" + textureFile;

	std::shared_ptr<ImageTexture2D> imageTexture =
		std::make_shared<ImageTexture2D>(texturePath);

	std::shared_ptr<Material> material =
		std::make_shared<Material>
		(
			vertexShader,
			pixelShader,
			imageTexture
		);

	material->SetAmbientCoefficient(
		OBJmaterial.vAmbient.x,
		OBJmaterial.vAmbient.y,
		OBJmaterial.vAmbient.z
	);
	material->SetDiffuseCoefficient(
		OBJmaterial.vDiffuse.x,
		OBJmaterial.vDiffuse.y,
		OBJmaterial.vDiffuse.z
	);
	material->SetSpecularCoefficient(
		OBJmaterial.vSpecular.x,
		OBJmaterial.vSpecular.y,
		OBJmaterial.vSpecular.z
	);

	float phongExponent = (float)OBJmaterial.nShininess;
	if (OBJmaterial.nShininess == 0)
	{
		phongExponent = 1;
		material->SetSpecularCoefficient(0, 0, 0);
	}
	material->SetPhongExponent(phongExponent);

	SetMaterial(material);
}

OBJModel::~OBJModel()
{
	Logger::Info("OBJ Model Destructor!");
}