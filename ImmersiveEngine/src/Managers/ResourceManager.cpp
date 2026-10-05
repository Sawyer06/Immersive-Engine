#include "ResourceManager.h"

namespace ImmersiveEngine
{
	std::vector<std::shared_ptr<Rendering::Mesh>> ResourceManager::loadFromModel(std::string modelFile)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelFile, aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_GenNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Handle import errors.
		{
			std::cout << "ASSIMP_IMPORT_ERROR " << importer.GetErrorString() << "\n";
			std::vector<std::shared_ptr<Rendering::Mesh>> empty;
			return empty;
		}

		std::vector<std::shared_ptr<Rendering::Mesh>> modelMeshes;
		processNode(scene->mRootNode, scene, &modelMeshes);

		return modelMeshes;
	}

	void ResourceManager::processNode(aiNode* node, const aiScene* scene, std::vector<std::shared_ptr<Rendering::Mesh>>* o_meshes)
	{
		// Create all the meshes, including their materials and textures, associated with a node.
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// Convert the mesh.
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::shared_ptr<Rendering::Mesh> ieMesh = std::make_shared<Rendering::Mesh>(mesh);
			m_loadedResources.insert({ mesh->mName.C_Str(), ieMesh });

			o_meshes->push_back(ieMesh);

			// Convert the material.
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			//std::shared_ptr<ImmersiveEngine::Rendering::Material> ieMat = std::make_shared<ImmersiveEngine::Rendering::Material>();
			//m_loadedResources.insert({ mat->GetName().C_Str(), ieMat});

			// Convert the diffuse texture.
			aiString path;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path))
			{
				std::shared_ptr<ImmersiveEngine::Rendering::Texture> ieTex = std::make_shared<ImmersiveEngine::Rendering::Texture>(path.C_Str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
				m_loadedResources.insert({ path.C_Str(), ieTex });
			}
		}

		// Continue to process all nodes recursively.
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, o_meshes);
		}
	}


}