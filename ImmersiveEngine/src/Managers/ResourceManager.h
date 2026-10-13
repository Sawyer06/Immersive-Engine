#ifndef RESOURCE_MANAGER_CLASS_H
#define RESOURCE_MANAGER_CLASS_H

#include<iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"../Application/Resource.h"
#include"../Rendering/Mesh.h"
#include"../Rendering/Model.h"
#include"../Rendering/Material.h"
#include"../Settings.h"
#include "../Rendering/shaderClass.h"

namespace ImmersiveEngine
{
	class ResourceManager
	{
		public:
			ResourceManager() = default;
			~ResourceManager() = default;

			std::shared_ptr<Rendering::Model> loadFromModel(Rendering::Shader& shaderProgram, std::string modelFile, bool genNormals = false, bool flipFaces = false);
		private:
			std::unordered_map<std::string, std::shared_ptr<Resource>> m_loadedResources;

			void processNode(Rendering::ModelNode& modelRootNode, aiNode* assimpRootNode, const aiScene* scene, Rendering::Shader& shaderProgram);
			Rendering::Mesh assimpToEngineMesh(aiMesh* mesh);
	};		
}
#endif