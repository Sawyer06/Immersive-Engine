#ifndef RIGIDBODY_CLASS_H
#define RIGIDBODY_CLASS_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include"../Physics/ColliderShape.h"
#include"../Physics/ContactHandler.h"
#include"../Objects/GameObject.h"
#include"Space.h"

namespace ImmersiveEngine::cbs
{
	class RigidBody : public Component
	{
		private:
			JPH::BodyID m_ID;
			JPH::BodyInterface* m_bodyInterface = nullptr;

			JPH::BodyCreationSettings m_bodyCreationSettings;
			std::shared_ptr<ImmersiveEngine::Physics::ColliderShape> m_colliderShape;

			Space* m_ownerSpace;
		public:
			enum MotionType
			{
				Static,
				Kinematic,
				Dynamic
			};

			RigidBody(Object* obj, std::shared_ptr<ImmersiveEngine::Physics::ColliderShape> colliderShape, MotionType motionType = MotionType::Static);
			~RigidBody() = default;

			bool isSensor = false;
			MotionType motionType = MotionType::Static;
			float restitution = 0.0f;
			bool autoActivate = false; // Should stay false for physics optimazation unless necessary.

			void initialize(JPH::BodyInterface* bodyInterface);
			
			bool isActive();
			JPH::BodyID getBodyID();

			ImmersiveEngine::Math::Vector3 getLinearVelocity();
			void setLinearVelocity(ImmersiveEngine::Math::Vector3 amnt);
			ImmersiveEngine::Math::Vector3 getAngularVelocity();
			void setAngularVelocity(ImmersiveEngine::Math::Vector3 amnt);

			void addForce(ImmersiveEngine::Math::Vector3 amnt);
			void addImpulse(ImmersiveEngine::Math::Vector3 amnt);
			void addTorque(ImmersiveEngine::Math::Vector3 amnt);

			void refreshRigidBody();

			void dump();

			std::string toString() override;
	};
}
#endif
