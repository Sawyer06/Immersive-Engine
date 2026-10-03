#include "ResourceManager.h"

namespace ImmersiveEngine
{
	std::vector<std::shared_ptr<Rendering::Mesh>> ResourceManager::loadFromModel(std::string modelFile)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelFile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes);

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
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			
			std::shared_ptr<Rendering::Mesh> ieMesh = std::make_shared<Rendering::Mesh>(mesh);
			m_loadedMeshes.push_back(ieMesh);
			o_meshes->push_back(ieMesh);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, o_meshes);
		}
	}


}