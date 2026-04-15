#include "graphics/models/objmodel.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/meshes/mesh.h"
#include "graphics/textures/imagetexture2d.h"
#include "graphics/materials/material.h"

#include "core/logger.h"

#include <filesystem>

#include "tinyobjloader/tiny_obj_loader.h"

OBJModel::OBJModel(
	const std::string& path,
	std::shared_ptr<VertexShader> vertexShader,
	std::shared_ptr<PixelShader> pixelShader,
	const bool isLeftHanded
)
{
	std::vector<size_t> meshesIndicies;

	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<uint32_t>> indicies;
	std::vector<std::string> names;

	tinyobj::ObjReaderConfig config;
	config.mtl_search_path = "";

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(path, config))
	{
		if (!reader.Error().empty())
		{
			Logger::Error("TinyOBJ failed to parse file: " + path + ", reason: " + reader.Error());
			// TODO: Maybe add fallback model here
			throw std::runtime_error("");
		}
	}

	if (!reader.Warning().empty())
	{
		Logger::Warn("TinyOBJ warning: " + reader.Warning());
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& objMaterials = reader.GetMaterials();

	for (size_t shapeIndex = 0; shapeIndex < shapes.size(); ++shapeIndex)
	{
		vertices.emplace_back(std::vector<Vertex>());
		indicies.emplace_back(std::vector<uint32_t>());

		auto& shape = shapes[shapeIndex];
		size_t nFaces = shape.mesh.num_face_vertices.size();

		names.emplace_back(shape.name);

		// Assume every face in the same shape shares the same material, maybe something to think about later.
		meshesIndicies.push_back(shape.mesh.material_ids[0]);

		size_t indexOffset = 0;
		for (size_t faceIndex = 0; faceIndex < nFaces; ++faceIndex)
		{

			if (shape.mesh.material_ids[0] != shape.mesh.material_ids[faceIndex])
			{
				Logger::Warn("Mesh " + shape.name + " has multiple materials");
			}

			size_t nVerticesInFace = size_t(shape.mesh.num_face_vertices[faceIndex]);
			for (size_t vertexIndex = 0; vertexIndex < nVerticesInFace; ++vertexIndex)
			{
				tinyobj::index_t idx = shape.mesh.indices[indexOffset + vertexIndex];
				//tinyobj::index_t idx = shape.mesh.indices[vertexIndex];
				tinyobj::real_t x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				if (idx.normal_index < 0)
				{
					Logger::Error("Loading mesh has no normal data");
				}

				tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
				tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
				tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

				if (idx.texcoord_index < 0)
				{
					Logger::Error("Loading mesh has no uv data");
				}

				tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

				if (isLeftHanded)
				{
					ty = 1 - ty;
				}

				vertices[shapeIndex].push_back(
					{
						{x, y, z},
						{nx, ny, nz},
						{tx, ty}
					}
				);

				indicies[shapeIndex].push_back(
					static_cast<uint32_t>(vertices[shapeIndex].size() - 1)
				);
			}
			indexOffset += nVerticesInFace;
		}
	}

	size_t materialCount = objMaterials.size();
	for (size_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		auto& objMaterial = objMaterials[materialIndex];

		std::string dir = std::filesystem::path(path)
			.parent_path()
			.string();

		std::string texturePath = dir + "/" + objMaterial.diffuse_texname;

		std::shared_ptr<ImageTexture2D> texture = std::make_shared<ImageTexture2D>(texturePath);

		std::shared_ptr<Material> material = std::make_shared<Material>(vertexShader, pixelShader, texture);

		material->SetAmbientCoefficient(objMaterial.ambient[0], objMaterial.ambient[1], objMaterial.ambient[2]);
		material->SetDiffuseCoefficient(objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2]);
		material->SetSpecularCoefficient(objMaterial.specular[0], objMaterial.specular[1], objMaterial.specular[2]);

		float phongExponent = objMaterial.shininess;
		if (phongExponent == 0)
		{
			phongExponent = 1;
			material->SetSpecularCoefficient(0, 0, 0);
		}
		material->SetPhongExponent(phongExponent);
		material->SetName(objMaterial.name);

		AddMaterial(material);
	}

	size_t meshCount = vertices.size();
	for (size_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Create(vertices[meshIndex], indicies[meshIndex]);
		mesh->SetName(names[meshIndex]);

		AddMesh(mesh, meshesIndicies[meshIndex]);
	}
}

OBJModel::~OBJModel()
{
}