#ifndef RESOURCE_MANAGER_CLASS_H
#define RESOURCE_MANAGER_CLASS_H

#include<iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Rendering/Mesh.h"

namespace ImmersiveEngine
{
	class ResourceManager
	{
		public:
			ResourceManager() = default;
			~ResourceManager() = default;

			std::vector<std::shared_ptr<Rendering::Mesh>> loadFromModel(std::string modelFile);
		private:
			std::vector<std::shared_ptr<Rendering::Mesh>> m_loadedMeshes;

			void processNode(aiNode* node, const aiScene* scene, std::vector<std::shared_ptr<Rendering::Mesh>>* o_meshes);
	};
}
#endif