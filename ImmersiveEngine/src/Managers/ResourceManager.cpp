#include "ResourceManager.h"

namespace ImmersiveEngine
{
	std::shared_ptr<Rendering::Model> ResourceManager::loadFromModel(Rendering::Shader& shaderProgram, std::string modelFile, bool genNormals, bool flipFaces)
	{
		Assimp::Importer importer;

		// Assign flags for model importing.
		unsigned int flags = aiProcess_Triangulate;
		if (genNormals)
			flags |= aiProcess_GenNormals;
		if (!flipFaces)
			flags |= aiProcess_FlipWindingOrder;

		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); // Needed to remove unnecessary nodes.
		const aiScene* scene = importer.ReadFile(modelFile, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Handle import errors.
		{
			std::cout << "ASSIMP_IMPORT_ERROR " << importer.GetErrorString() << "\n";
			return nullptr;
		}

		std::shared_ptr<Rendering::Model> model = std::make_shared<Rendering::Model>();
		processNode(model->getRoot(), scene->mRootNode, model, scene, shaderProgram);

		// Create all animations on the model.
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			std::shared_ptr<Rendering::AnimationClip> clip = std::make_shared<Rendering::AnimationClip>(assimpToEngineAnimClip(scene->mAnimations[i], model));
			model->animations.push_back(clip);
			m_loadedResources.insert({ scene->mAnimations[i]->mName.C_Str(), clip});
		}

		m_loadedResources.insert({ scene->mName.C_Str(), model });
		return model;
	}

	void ResourceManager::processNode(Rendering::ModelNode& modelRootNode, aiNode* assimpRootNode,
									std::shared_ptr<Rendering::Model> model, const aiScene* scene, Rendering::Shader& shaderProgram)
	{
		Rendering::ModelNode& currNode = modelRootNode.children.emplace_back();
		currNode.name = assimpRootNode->mName.C_Str();
		currNode.localTransform = Math::assimpToGLMMat4(assimpRootNode->mTransformation);

		// Create all the meshes, including their materials and textures, associated with a node.
		for (int i = 0; i < assimpRootNode->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[assimpRootNode->mMeshes[i]];

			// Convert the mesh.
			std::shared_ptr<Rendering::Mesh> engineMesh = std::make_shared<Rendering::Mesh>(assimpToEngineMesh(mesh));
			m_loadedResources.insert({ mesh->mName.C_Str(), engineMesh });

			if (mesh->HasBones())
				associateMeshBones(mesh, model, engineMesh);

			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			// Convert the diffuse texture.
			std::shared_ptr<ImmersiveEngine::Rendering::Texture> engineDiffuseTex = nullptr;
			aiString path;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				std::filesystem::path file(path.C_Str());
				engineDiffuseTex = std::make_shared<ImmersiveEngine::Rendering::Texture>(
					file.filename().string().c_str(), // Remove the path, just get the file name.
					GL_TEXTURE_2D, GL_TEXTURE0, 
					GL_UNSIGNED_BYTE);
				m_loadedResources.insert({ path.C_Str(), engineDiffuseTex });
			}

			std::shared_ptr<Rendering::Material> engineMaterial = std::make_shared<Rendering::Material>(shaderProgram, engineDiffuseTex);
			
			aiColor4D color;
			if (engineDiffuseTex == nullptr && mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) // Check for material color if no texture found.
			{
				engineMaterial->mainColor = Math::Vector3(color.r, color.g, color.b);
			}

			m_loadedResources.insert({ mat->GetName().C_Str(), engineMaterial });

			currNode.pairs.push_back({ engineMesh, engineMaterial }); // Create a new MeshMat pair of the newly created mesh and material.
		}

		// Continue to process all nodes recursively.
		for (int i = 0; i < assimpRootNode->mNumChildren; i++)
		{
			processNode(currNode, assimpRootNode->mChildren[i], model, scene, shaderProgram);
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

	/// Rig up a mesh with its bones and weights.
	void ResourceManager::associateMeshBones(aiMesh* mesh, std::shared_ptr<Rendering::Model> model, std::shared_ptr<Rendering::Mesh> engineMesh)
	{
		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
		{
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

			// Find node with the same name as the bone.
			auto boneNode = model->findNode(boneName);
			if (boneNode == nullptr)
			{
				continue;
			}

			if (boneNode->boneID <= -1) // Bone doesn't exist, create a new one.
			{
				boneNode->boneID = model->getNextBoneID();
				boneNode->localOffset = Math::assimpToGLMMat4(mesh->mBones[boneIndex]->mOffsetMatrix);
			}
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			// Set all vertex influences associated with the bone.
			for (int i = 0; i < numWeights; i++)
			{
				engineMesh->setBoneAssociation(weights[i].mVertexId, boneNode->boneID, weights[i].mWeight);
			}
		}
		engineMesh->buildMesh();
	}

	Rendering::AnimationClip ResourceManager::assimpToEngineAnimClip(aiAnimation* anim, std::shared_ptr<Rendering::Model> model)
	{
		Rendering::AnimationClip clip(anim->mDuration, anim->mTicksPerSecond);

		// Initialize all key rows.
		clip.positionKeys.resize(100);
		clip.orientationKeys.resize(100);
		clip.scaleKeys.resize(100);

		// Create keys for every node in model affected by animation.
		for (int i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim* currChannel = anim->mChannels[i];
			// Get the bone associated with the channel of same name.
			int boneID = -1;
			auto node = model->findNode(currChannel->mNodeName.data);
			if (node != nullptr)
				boneID = node->boneID;

			if (boneID <= -1)
				continue;

			glm::vec3 originalScale;
			glm::quat originalOrientation;
			glm::vec3 originalPosition;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(node->localTransform, originalScale, originalOrientation, originalPosition, skew, perspective);

			// Populate clip with position keys.
			for (int posIndex = 0; posIndex < currChannel->mNumPositionKeys; posIndex++)
			{
				auto position = currChannel->mPositionKeys[posIndex].mValue;

				clip.positionKeys[boneID].push_back(Rendering::Vector3Key{
					(float)currChannel->mPositionKeys[posIndex].mTime,
					Math::Vector3(position.x, position.y, position.z)
					});
			}

			// Create a default key if none exist.
			if (clip.positionKeys[boneID].size() == 0)
				clip.positionKeys[boneID].push_back(Rendering::Vector3Key{ 0.0f, 
					Math::Vector3(originalPosition.x, originalPosition.y, originalPosition.z)
					});

			// Populate clip with orientation keys.
			for (int rotIndex = 0; rotIndex < currChannel->mNumRotationKeys; rotIndex++)
			{
				auto orientation = currChannel->mRotationKeys[rotIndex].mValue;

				clip.orientationKeys[boneID].push_back(Rendering::QuaternionKey{
					(float)currChannel->mRotationKeys[rotIndex].mTime,
					Math::Quaternion(orientation.w, orientation.x, orientation.y, orientation.z)
					});
			}
			
			// Create a default key if none exist.
			if (clip.orientationKeys[boneID].size() == 0)
				clip.orientationKeys[boneID].push_back(Rendering::QuaternionKey{ 0.0f,
					Math::Quaternion(originalOrientation.w, originalOrientation.x, originalOrientation.y, originalOrientation.z)
					});

			// Populate clip with scale keys.
			for (int scaleIndex = 0; scaleIndex < currChannel->mNumScalingKeys; scaleIndex++)
			{
				auto scale = currChannel->mScalingKeys[scaleIndex].mValue;

				clip.scaleKeys[boneID].push_back(Rendering::Vector3Key{
					(float)currChannel->mScalingKeys[scaleIndex].mTime,
					Math::Vector3(scale.x, scale.y, scale.z)
					});
			}

			// Create a default key if none exist.
			if (clip.scaleKeys[boneID].size() == 0)
				clip.scaleKeys[boneID].push_back(Rendering::Vector3Key{ 0.0f,
					Math::Vector3(originalScale.x, originalScale.y, originalScale.z)
					});
		}

		return clip;
	}
}