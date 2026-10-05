#include"MeshRenderer.h"

namespace ImmersiveEngine::cbs
{
	ImmersiveEngine::cbs::MeshRenderer::MeshRenderer(Object* obj) :
		Component(obj)
	{

	}

	ImmersiveEngine::cbs::MeshRenderer::MeshRenderer(Object* obj, std::shared_ptr<Rendering::Model> model) :
		Component(obj), m_model(model)
	{

	}
	

}