#pragma once
#include <memory>

class Material;
class Mesh;
class RenderServer;

class Model
{
public:
	Model();
	Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	~Model();

	void SetMesh(std::shared_ptr<Mesh> mesh);
	void SetMaterial(std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh() { return mMesh; }
	std::shared_ptr<Material> GetMaterial() { return mMaterial; }

	size_t GetNumIndicies();

	void RenderImgui();

private:
	std::shared_ptr<Mesh> mMesh;
	std::shared_ptr<Material> mMaterial;
};