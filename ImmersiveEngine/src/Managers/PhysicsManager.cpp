#include "PhysicsManager.h"

namespace ImmersiveEngine::cbs
{
	void PhysicsManager::dump()
	{
		for (ImmersiveEngine::cbs::RigidBody* rb : m_rigidBodies)
		{
			rb->dump();
		}

		JPH::UnregisterTypes();
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}
	void PhysicsManager::initialize()
	{
		JPH::RegisterDefaultAllocator();
		JPH::Trace = IETrace;
#ifdef JPH_ENABLE_ASSERTS
		JPH::AssertFailed = IEAssertFailed;
#endif
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();
		m_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
		m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

		m_physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, 
			m_broadPhaseLayerInterface, m_objectVsBroadphaseLayerFilter, m_objectVsObjectLayerFilter);

		m_physicsSystem.SetBodyActivationListener(&m_bodyActivationListener);
		m_physicsSystem.SetContactListener(&m_contactListener);

		m_bodyInterface = &m_physicsSystem.GetBodyInterface();

		std::vector<RigidBody*> rigidBodiesToInitialize = m_rigidBodies;

		// Initialize all rigid bodies attached to constraints first to get reference to body object.
		for (ImmersiveEngine::Physics::Constraint* constraint : m_constraints)
		{
			// Initiailze rigid body 1.
			JPH::BodyCreationSettings bodyCreationSettings1 = constraint->rb1->getBodyCreationSettings();

			JPH::Body& body1 = *m_bodyInterface->CreateBody(bodyCreationSettings1);
			
			JPH::BodyID ID1 = body1.GetID();
			m_bodyInterface->AddBody(ID1, constraint->rb1->autoActivate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);

			constraint->rb1->initialize(m_bodyInterface, ID1);
			rigidBodiesToInitialize.erase(std::find(rigidBodiesToInitialize.begin(), rigidBodiesToInitialize.end(), constraint->rb1));

			// Initialize rigid body 2.
			JPH::BodyCreationSettings bodyCreationSettings2 = constraint->rb2->getBodyCreationSettings();

			JPH::Body& body2 = *m_bodyInterface->CreateBody(bodyCreationSettings2);

			JPH::BodyID ID2 = body2.GetID();
			m_bodyInterface->AddBody(ID2, constraint->rb2->autoActivate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);

			constraint->rb2->initialize(m_bodyInterface, ID2);
			rigidBodiesToInitialize.erase(std::find(rigidBodiesToInitialize.begin(), rigidBodiesToInitialize.end(), constraint->rb2));

			// Create the constraint and add to physics system.
			if (ImmersiveEngine::Physics::FixedConstraint* fixedConstraint = dynamic_cast<ImmersiveEngine::Physics::FixedConstraint*>(constraint))
			{
				JPH::FixedConstraintSettings settings;
				constraint->m_physicsConstraint = settings.Create(body1, body2);
				m_physicsSystem.AddConstraint(constraint->m_physicsConstraint);
			}
		}

		// Initialize the remaining rigid bodies.
		for (RigidBody* rb : rigidBodiesToInitialize)
		{
			JPH::BodyCreationSettings bodyCreationSettings = rb->getBodyCreationSettings();

			JPH::BodyID ID = m_bodyInterface->CreateAndAddBody(bodyCreationSettings, rb->autoActivate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);

			rb->initialize(m_bodyInterface, ID);
		}
	}


	void PhysicsManager::addRigidBody(ImmersiveEngine::cbs::RigidBody* rb)
	{
		m_rigidBodies.push_back(rb);
	}

	void PhysicsManager::removeRigidBody(uint32_t index)
	{
		if (index >= m_rigidBodies.size()) return;

		m_rigidBodies[index]->dump();
		m_rigidBodies.erase(m_rigidBodies.begin() + index);
	}

	void PhysicsManager::addConstraint(ImmersiveEngine::Physics::Constraint* constraint)
	{
		m_constraints.push_back(constraint);
	}

	void PhysicsManager::removeConstraint(uint32_t index)
	{
		if (index >= m_constraints.size()) return;

		m_physicsSystem.RemoveConstraint(m_constraints[index]->m_physicsConstraint);
		m_constraints.erase(m_constraints.begin() + index);
	}

	void PhysicsManager::refreshBodies()
	{
		for (ImmersiveEngine::cbs::RigidBody* rb : m_rigidBodies)
		{
			rb->refreshRigidBody();
		}
	}

	void PhysicsManager::onStart()
	{
		initialize();
	}

	void PhysicsManager::onUpdate(float deltaTime)
	{
		refreshBodies();

		m_physicsSystem.Update(deltaTime, 1, m_tempAllocator.get(), m_jobSystem.get());
	}
}