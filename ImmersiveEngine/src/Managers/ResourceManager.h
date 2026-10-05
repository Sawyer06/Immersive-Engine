#ifndef RESOURCE_MANAGER_CLASS_H
#define RESOURCE_MANAGER_CLASS_H

#include<iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"../Application/Resource.h"
#include "../Rendering/Mesh.h"
#include"../Rendering/Material.h"

namespace ImmersiveEngine
{
	class ResourceManager
	{
		public:
			ResourceManager() = default;
			~ResourceManager() = default;

			std::vector<std::shared_ptr<Rendering::Mesh>> loadFromModel(std::string modelFile);
		private:
			std::unordered_map<std::string, std::shared_ptr<Resource>> m_loadedResources;

			void processNode(aiNode* node, const aiScene* scene, std::vector<std::shared_ptr<Rendering::Mesh>>* o_meshes);
	};
}
#endif