#include "PhysicsManager.h"

namespace ImmersiveEngine::cbs
{
	void PhysicsManager::initialize()
	{
		JPH::RegisterDefaultAllocator();
		JPH::Trace = IETrace;
#ifdef JPH_ENABLE_ASSERTS
		JPH::AssertFailed = IEAssertFailed;
#endif
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();
		JPH::TempAllocatorImpl tempAllocator(10 * 1024 * 1024);
		JPH::JobSystemThreadPool jobSystem(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

		m_physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, 
			m_broadPhaseLayerInterface, m_objectVsBroadphaseLayerFilter, m_objectVsObjectLayerFilter);

		m_physicsSystem.SetBodyActivationListener(&m_bodyActivationListener);
		m_physicsSystem.SetContactListener(&m_contactListener);

		m_bodyInterface = &m_physicsSystem.GetBodyInterface();
	}

	void PhysicsManager::addRigidBody(RigidBody& rb)
	{
		JPH::BodyCreationSettings createSettings;

		m_bodyInterface->CreateAndAddBody(createSettings, JPH::EActivation::Activate);
		
		m_rigidBodies.push_back(rb);
	}

	void PhysicsManager::removeRigidBody(uint32_t index)
	{
		if (index > m_rigidBodies.size()) return;

		m_bodyInterface->RemoveBody(m_rigidBodies[index].getBodyID());
		m_rigidBodies.erase(m_rigidBodies.begin() + index);
	}

	RigidBody& PhysicsManager::getRigidBody(uint32_t index)
	{
		//if (index > m_rigidBodies.size()) return ;

		return m_rigidBodies[index];
	}

	void PhysicsManager::refreshBodies()
	{
		for (RigidBody rb : m_rigidBodies)
		{
			// Get center of mass
			// Set space->position to center of mass pos
		}

		// physicsSystem.update()
	}
}