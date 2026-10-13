#ifndef MESH_RENDERER_CLASS_H
#define MESH_RENDERER_CLASS_H

#include"../Objects/GameObject.h"
#include"../Rendering/Model.h"
#include"../Rendering/shaderClass.h"

namespace ImmersiveEngine::cbs
{
	class MeshRenderer : public Component
	{
		public:
			MeshRenderer(Object* obj);
			MeshRenderer(Object* obj, std::shared_ptr<Rendering::Model> model);
			~MeshRenderer() = default;

			void onUpdate() override;
		private:
			std::shared_ptr<Rendering::Model> m_model;

			void drawNode(Rendering::ModelNode root);
			void drawPair(std::shared_ptr<Rendering::Mesh> mesh, std::shared_ptr<Rendering::Material> material);
	};
}
#endif
