#ifndef MESH_RENDERER_CLASS_H
#define MESH_RENDERER_CLASS_H

#include"../Objects/GameObject.h"
#include"../Rendering/Model.h"

namespace ImmersiveEngine::cbs
{
	class MeshRenderer : public Component
	{
		public:
			MeshRenderer(Object* obj);
			MeshRenderer(Object* obj, std::shared_ptr<Rendering::Model> model);
			~MeshRenderer() = default;

		private:
			std::shared_ptr<Rendering::Model> m_model;
	};
}
#endif
