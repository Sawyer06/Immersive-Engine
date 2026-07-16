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

			// Must call physics functions (addForce(), addTorque(), etc) from here as calling it from a Body will not wake it up from sleep.
			JPH::BodyInterface* m_bodyInterface = nullptr;

			JPH::BodyCreationSettings m_bodyCreationSettings;
			std::shared_ptr<ImmersiveEngine::Physics::ColliderShape> m_colliderShape;

			Space* m_ownerSpace;

			JPH::BodyID getBodyID();
			JPH::BodyCreationSettings getBodyCreationSettings();
		public:
			enum MotionType
			{
				Static,		// Non movable.
				Kinematic,	// Movable using velocities only, does not respond to forces.
				Dynamic		// Responds to forces as a normal physics object.
			};

			RigidBody(Object* obj, std::shared_ptr<ImmersiveEngine::Physics::ColliderShape> colliderShape, MotionType motionType = MotionType::Static);
			~RigidBody() = default;

			bool isSensor = false;
			MotionType motionType = MotionType::Static;
			float restitution = 0.0f;
			bool autoActivate = false; // Should stay false for physics optimazation unless necessary.

			void initialize(JPH::BodyInterface* bodyInterface, JPH::BodyID ID);
			
			bool isActive();

			void setPositionAndOrientation(ImmersiveEngine::Math::Vector3 position, ImmersiveEngine::Math::Quaternion orientation);

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

			friend class PhysicsManager;
	};
}
#endif
