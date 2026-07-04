#ifndef CONTACTHANDLER_CLASS_H
#define CONTACTHANDLER_CLASS_H

#include"../Objects/Present.h"

namespace ImmersiveEngine::Physics
{
	struct ContactInfo
	{
		ImmersiveEngine::cbs::Present* gameObject;

		ImmersiveEngine::Math::Vector3 baseOffset;
		std::vector<ImmersiveEngine::Math::Vector3> relativeContactPointsPosition;
		std::vector<ImmersiveEngine::Math::Vector3> worldContactPointsPosition;

		float penetrationDepth;
	};

	/// Must be parent to the component in which you want to detect contacts.
	class ContactHandler
	{
		public:
			virtual void OnContactBegan(ContactInfo contact) {};
			virtual void OnContactPersisted(ContactInfo contact) {};
			virtual void OnContactRemoved() {};
	};
}
#endif