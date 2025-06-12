#include"Component.h"

namespace ImmersiveEngine::cbs
{
	Component::Component() :
		m_owner(std::weak_ptr<ImmersiveEngine::cbs::Object>()) { }

	Component::Component(const std::shared_ptr<ImmersiveEngine::cbs::Object> obj) :
		m_owner(obj) { }

	/// Retrieve the owner of the component for access to it.
	std::shared_ptr<ImmersiveEngine::cbs::Object> Component::getOwner() const
	{
		auto obj = m_owner.lock();
		if (!obj) // Owner has abandonded this component.
		{
			std::cerr << "NULL_REFERENCE_ERROR component has no owner.";
		}
		return obj;
	}

	/// Change ownership of a component.
	void Component::transferOwnership(const std::shared_ptr<Object> newObj)
	{
		m_owner = newObj;
	}
}