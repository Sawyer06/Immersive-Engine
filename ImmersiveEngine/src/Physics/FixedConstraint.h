#ifndef FIXED_CONSTRAINT_CLASS_H
#define FIXED_CONSTRAINT_CLASS_H

#include "Constraint.h"

namespace ImmersiveEngine::Physics
{
	class FixedConstraint : public Constraint
	{
		public:
			FixedConstraint(ImmersiveEngine::cbs::RigidBody* rb1, ImmersiveEngine::cbs::RigidBody* rb2) :
				Constraint(rb1, rb2) { }
	};
}
#endif
