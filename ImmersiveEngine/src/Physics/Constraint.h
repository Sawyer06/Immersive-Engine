#ifndef CONSTRAINT_CLASS_H
#define CONSTRAINT_CLASS_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>

#include "../Components/RigidBody.h"

namespace ImmersiveEngine::Physics
{
	class ImmersiveEngine::cbs::PhysicsManager;

	class Constraint
	{
		protected:
			Constraint(ImmersiveEngine::cbs::RigidBody* rb1, ImmersiveEngine::cbs::RigidBody* rb2) :
				rb1(rb1), rb2(rb2) { }
		
		private:
			JPH::Constraint* m_physicsConstraint = nullptr;
		
		public:
			virtual ~Constraint() = default;

			ImmersiveEngine::cbs::RigidBody* rb1;
			ImmersiveEngine::cbs::RigidBody* rb2;

			friend class ImmersiveEngine::cbs::PhysicsManager;
	};
}
#endif
