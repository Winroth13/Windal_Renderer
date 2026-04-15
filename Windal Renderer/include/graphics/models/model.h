#pragma once
#include <memory>
#include <vector>

class Material;
class Mesh;
class RenderServer;

class Model
{
public:
	Model();
	Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	~Model();

	void AddMesh(std::shared_ptr<Mesh> mesh, size_t materialIndex = 0);
	void AddMaterial(std::shared_ptr<Material> material);

	std::vector<std::shared_ptr<Mesh>> GetMeshes() { return mMeshes; }
	std::vector<std::shared_ptr<Material>> GetMaterials() { return mMaterials; }

	std::shared_ptr<Mesh> GetMesh(size_t index) { return mMeshes[index]; }
	std::shared_ptr<Material> GetMaterial(size_t index) { return mMaterials[mMaterialIndicies[index]]; }

	size_t GetNumIndicies(size_t index);
	const size_t GetMeshCount() { return mMeshes.size(); }

	const bool IsMeshVisible(const size_t index) { return mMeshVisibility[index]; }

	void RenderImgui();

private:
	std::vector<std::shared_ptr<Mesh>> mMeshes;
	std::vector<size_t> mMaterialIndicies;
	std::vector<std::shared_ptr<Material>> mMaterials;

	std::vector<bool> mMeshVisibility;
};