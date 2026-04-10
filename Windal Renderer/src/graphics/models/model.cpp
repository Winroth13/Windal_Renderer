#include "graphics/models/model.h"
#include "core/renderserver.h"
#include "graphics/meshes/mesh.h"
#include "graphics/materials/material.h"

#include "core/logger.h"

#include "core/imguiflags.h"
#include "imgui/imgui.h"

Model::Model()
	: mMesh(nullptr), mMaterial(nullptr)
{
}

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
	: mMesh(mesh), mMaterial(material)
{
}

Model::~Model()
{
	Logger::Info("Model Destructor!");
};

void Model::SetMesh(std::shared_ptr<Mesh> mesh)
{
	mMesh = mesh;
}

void Model::SetMaterial(std::shared_ptr<Material> material)
{
	mMaterial = material;
}

size_t Model::GetNumIndicies()
{
	return mMesh->GetNumIndicies();
}

void Model::RenderImgui()
{
	if (ImGui::TreeNodeEx("Model", TREE_NODE_FLAGS))
	{
		if (ImGui::TreeNodeEx("Material", TREE_NODE_FLAGS))
		{
			mMaterial->RenderImgui();
			ImGui::TreePop();
		}
			
		if (ImGui::TreeNodeEx("Mesh", TREE_NODE_FLAGS))
		{
			mMesh->RenderImgui();
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}