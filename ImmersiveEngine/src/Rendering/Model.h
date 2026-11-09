#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<iostream>
#include <vector>

#include <glm/glm.hpp>
#include"Mesh.h"
#include"Material.h"
#include"AnimationClip.h"

namespace ImmersiveEngine::Rendering
{
	struct MeshMatPair
	{
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	struct ModelNode
	{
		std::string name;

		std::vector<MeshMatPair> pairs;
		std::vector<ModelNode> children;

		int boneID = -1;

		glm::mat4 localTransform = glm::mat4(1.0f);

		glm::mat4 localOffset = glm::mat4(1.0f);
	};

	class Model : public Resource
	{
		public:
			Model() = default;
			~Model() = default;

			std::vector<std::shared_ptr<Rendering::AnimationClip>> animations;

			ModelNode& getRoot() { return m_root; }
			ModelNode* findNode(std::string name) { return findNode(name, getRoot()); }

			int getNextBoneID() { return nextBoneID++; }

		private:
			ModelNode m_root;

			int nextBoneID = 0;

			ModelNode* findNode(std::string name, ModelNode& currNode)
			{
				if (currNode.name == name)
				{
					return &currNode;
				}
				else
				{
					for (auto& node : currNode.children)
					{
						if (auto found = findNode(name, node))
							return found;
					}
				}
				return nullptr;
			}
	};
}
#endif
