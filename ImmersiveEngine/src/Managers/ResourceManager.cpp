#include "ResourceManager.h"

namespace ImmersiveEngine
{
	std::shared_ptr<Rendering::Model> ResourceManager::loadFromModel(Rendering::Shader& shaderProgram, std::string modelFile, bool genNormals, bool flipFaces)
	{
		Assimp::Importer importer;

		unsigned int flags = aiProcess_Triangulate;
		if (genNormals)
			flags |= aiProcess_GenNormals;
		if (!flipFaces)
			flags |= aiProcess_FlipWindingOrder;

		const aiScene* scene = importer.ReadFile(modelFile, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Handle import errors.
		{
			std::cout << "ASSIMP_IMPORT_ERROR " << importer.GetErrorString() << "\n";
			return nullptr;
		}

		std::shared_ptr<Rendering::Model> model = std::make_shared<Rendering::Model>();
		processNode(model->getRoot(), scene->mRootNode, scene, shaderProgram);

		return model;
	}

	void ResourceManager::processNode(Rendering::ModelNode& modelRootNode, aiNode* assimpRootNode, const aiScene* scene, Rendering::Shader& shaderProgram)
	{
		Rendering::ModelNode& currNode = modelRootNode.children.emplace_back();
		currNode.name = assimpRootNode->mName.C_Str();

		// Create all the meshes, including their materials and textures, associated with a node.
		for (unsigned int i = 0; i < assimpRootNode->mNumMeshes; i++)
		{
			// Convert the mesh.
			aiMesh* mesh = scene->mMeshes[assimpRootNode->mMeshes[i]];
			std::shared_ptr<Rendering::Mesh> engineMesh = std::make_shared<Rendering::Mesh>(assimpToEngineMesh(mesh));
			m_loadedResources.insert({ mesh->mName.C_Str(), engineMesh });

			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			// Convert the diffuse texture.
			std::shared_ptr<ImmersiveEngine::Rendering::Texture> engineDiffuseTex = nullptr;
			aiString path;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path))
			{
				engineDiffuseTex = std::make_shared<ImmersiveEngine::Rendering::Texture>(path.C_Str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
				m_loadedResources.insert({ path.C_Str(), engineDiffuseTex });
			}

			std::shared_ptr<Rendering::Material> engineMaterial = std::make_shared<Rendering::Material>(shaderProgram, engineDiffuseTex);
			m_loadedResources.insert({ mat->GetName().C_Str(), engineMaterial });

			currNode.pairs.push_back({ engineMesh, engineMaterial }); // Create a new MeshMat pair of the newly created mesh and material.
		}

		// Continue to process all nodes recursively.
		for (unsigned int i = 0; i < assimpRootNode->mNumChildren; i++)
		{
			processNode(currNode, assimpRootNode->mChildren[i], scene, shaderProgram);
		}
	}

	/// Converts an assimp mesh to an engine mesh.
	Rendering::Mesh ResourceManager::assimpToEngineMesh(aiMesh* mesh)
	{
		std::vector<Rendering::Vertex> vertices;
		std::vector<GLuint> indices;

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			ImmersiveEngine::Math::Vector3 normals;
			if (mesh->mNormals)
				normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			ImmersiveEngine::Math::Vector2 uvCoords;
			if (mesh->mTextureCoords[0])
				uvCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			vertices.push_back(Rendering::Vertex{
				ImmersiveEngine::Math::Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
				normals,
				Settings::g_defaultMeshColor,
				uvCoords } // Need to make check for missing uv coords.
				);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Rendering::Mesh(vertices, indices);
	}
}