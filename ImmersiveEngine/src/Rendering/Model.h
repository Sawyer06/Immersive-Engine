#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<iostream>
#include <vector>

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
	};

	class Model : public Resource
	{
		public:
			Model(aiScene* scene);
			~Model() = default;

		private:
			ModelNode m_root;

	};
}
#endif
