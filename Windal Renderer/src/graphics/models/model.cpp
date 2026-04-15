#include "graphics/models/model.h"
#include "core/renderserver.h"
#include "graphics/meshes/mesh.h"
#include "graphics/materials/material.h"

#include "core/logger.h"

#include "core/imguiflags.h"
#include "imgui/imgui.h"

Model::Model()
{
}

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	AddMesh(mesh);
	AddMaterial(material);
}

Model::~Model()
{
	Logger::Info("Model Destructor!");
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh, size_t materialIndex)
{
	mMeshes.emplace_back(mesh);
	mMaterialIndicies.push_back(materialIndex);
}

void Model::AddMaterial(std::shared_ptr<Material> material)
{
	mMaterials.emplace_back(material);
}

size_t Model::GetNumIndicies(size_t index)
{
	return mMeshes[index]->GetNumIndicies();
}

void Model::RenderImgui()
{
	if (ImGui::TreeNodeEx("Model", TREE_NODE_FLAGS))
	{
		if (ImGui::TreeNodeEx("Materials", TREE_NODE_FLAGS))
		{
			for (auto& material : mMaterials)
			{
				if (ImGui::TreeNodeEx(material->GetName().c_str(), ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_Framed))
				{
					material->RenderImgui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Meshes", TREE_NODE_FLAGS))
		{
			for (auto& mesh : mMeshes)
			{
				if (ImGui::TreeNodeEx(mesh->GetName().c_str(), ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_Framed))
				{
					mesh->RenderImgui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}