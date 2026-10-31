#include "ResourceManager.h"

namespace ImmersiveEngine
{
	std::shared_ptr<Rendering::Model> ResourceManager::loadFromModel(Rendering::Shader& shaderProgram, std::string modelFile, bool genNormals, bool flipFaces, bool boneless)
	{
		Assimp::Importer importer;

		// Assign flags for model importing.
		unsigned int flags = aiProcess_Triangulate;
		if (genNormals)
			flags |= aiProcess_GenNormals;
		if (!flipFaces)
			flags |= aiProcess_FlipWindingOrder;
		if (boneless)
			flags |= aiProcess_Debone;

		const aiScene* scene = importer.ReadFile(modelFile, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Handle import errors.
		{
			std::cout << "ASSIMP_IMPORT_ERROR " << importer.GetErrorString() << "\n";
			return nullptr;
		}

		std::shared_ptr<Rendering::Model> model = std::make_shared<Rendering::Model>();
		processNode(model->getRoot(), scene->mRootNode, scene, shaderProgram);

		// Create all animations on the model.
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			std::shared_ptr<Rendering::AnimationClip> clip = std::make_shared<Rendering::AnimationClip>(assimpToEngineAnimClip(model, scene->mAnimations[i]));
			m_loadedResources.insert({ scene->mAnimations[i]->mName.C_Str(), clip});
		}

		m_loadedResources.insert({ scene->mName.C_Str(), model });
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
				uvCoords }
				);
		}

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Rendering::Mesh(vertices, indices);
	}

	Rendering::AnimationClip ResourceManager::assimpToEngineAnimClip(std::shared_ptr<Rendering::Model> model, aiAnimation * anim)
	{
		Rendering::AnimationClip clip(anim->mDuration, anim->mTicksPerSecond);
		
		// Create keys for every node in model affected by animation.
		for (int i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim* currChannel = anim->mChannels[i];

			// Get the bone associated with channel.
			int boneID = model->findNode(currChannel->mNodeName.C_Str()).boneID;

			// Populate clip with position keys.
			for (int posIndex = 0; posIndex < currChannel->mNumPositionKeys; posIndex++)
			{
				auto position = currChannel->mPositionKeys[posIndex].mValue;

				clip.positionKeys.push_back(Rendering::Vector3Key{
					(float)currChannel->mPositionKeys[posIndex].mTime,
					boneID,
					Math::Vector3(position.x, position.y, position.z)
					});
			}

			// Populate clip with orientation keys.
			for (int rotIndex = 0; rotIndex < currChannel->mNumRotationKeys; rotIndex++)
			{
				auto orientation = currChannel->mRotationKeys[rotIndex].mValue;

				clip.orientationKeys.push_back(Rendering::QuaternionKey{
					(float)currChannel->mPositionKeys[rotIndex].mTime,
					boneID,
					Math::Quaternion(orientation.w, orientation.x, orientation.y, orientation.z)
					});
			}

			// Populate clip with scale keys.
			for (int scaleIndex = 0; scaleIndex < currChannel->mNumScalingKeys; scaleIndex++)
			{
				auto scale = currChannel->mScalingKeys[scaleIndex].mValue;

				clip.positionKeys.push_back(Rendering::Vector3Key{
					(float)currChannel->mPositionKeys[scaleIndex].mTime,
					boneID,
					Math::Vector3(scale.x, scale.y, scale.z)
					});
			}
		}

		return clip;
	}
}