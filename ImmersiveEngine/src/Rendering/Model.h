#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<iostream>
#include <vector>

#include <glm/glm.hpp>
#include"Mesh.h"
#include"Material.h"

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
		glm::mat4 offset;
		glm::mat4 localTransform;
	};

	class Model : public Resource
	{
		public:
			Model() = default;
			~Model() = default;

			ModelNode& getRoot() { return m_root; }
			ModelNode& findNode(std::string name) { return findNode(name, getRoot()); }

		private:
			ModelNode m_root;

			ModelNode& findNode(std::string name, ModelNode& currNode)
			{
				if (currNode.name == name)
				{
					return currNode;
				}
				else
				{
					for (auto& node : currNode.children)
					{
						findNode(name, node);
					}
				}
			}
	};
}
#endif
