#ifndef RIGIDBODY_CLASS_H
#define RIGIDBODY_CLASS_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include"../Objects/Present.h"
#include"Space.h"

namespace ImmersiveEngine::cbs
{
	class ContactHandler
	{
		public:
			virtual void OnContactBegan(ContactInfo& contact);
			virtual void OnContactPersisted(ContactInfo& contact);
			virtual void OnContactRemoved(ContactInfo& contact);
	};

	class RigidBody : public Component
	{
		private:
			JPH::BodyID m_ID;
			JPH::BodyInterface* m_bodyInterface;

			Space* m_ownerSpace;
		public:
			RigidBody(Object* obj);
			~RigidBody() = default;

			enum MotionType
			{
				Static,
				Kinematic,
				Dynamic
			};

			bool isSensor;
			MotionType motionType;
			float restitution;

			bool isActive();
			JPH::BodyID getBodyID();

			ImmersiveEngine::Math::Vector3 getLinearVelocity();
			void setLinearVelocity(ImmersiveEngine::Math::Vector3 amnt);
			ImmersiveEngine::Math::Vector3 getAngularVelocity();
			void setAngularVelocity(ImmersiveEngine::Math::Vector3 amnt);

			void addForce(ImmersiveEngine::Math::Vector3 amnt);
			void addImpulse(ImmersiveEngine::Math::Vector3 amnt);
			void addTorque(ImmersiveEngine::Math::Vector3 amnt);

			void reset();

			void refreshRigidBody();

			std::string toString() override;
	};
}
#endif
