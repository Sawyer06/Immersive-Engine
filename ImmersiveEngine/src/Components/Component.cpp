#include"Component.h"

namespace ImmersiveEngine::cbs
{
	Component::Component() :
		m_owner(nullptr) { }

	Component::Component(Object& obj) :
		m_owner(&obj) { }

	/// Retrieve the owner of the component for access to it.
	Object* Component::getOwner() const
	{
		/*auto obj = m_owner.lock();
		if (!obj) // Owner has abandonded this component.
		{
			std::cerr << "NULL_REFERENCE_ERROR component has no owner.";
		}*/
		return m_owner;
	}

	/// Change ownership of a component.
	void Component::transferOwnership(Object& newObj)
	{
		m_owner = &newObj;
	}
}