#include "PhysicsManager.h"

namespace ImmersiveEngine::cbs
{
	PhysicsManager::PhysicsManager()
	{
		initialize();
	}
	PhysicsManager::~PhysicsManager()
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
	}

	void PhysicsManager::addRigidBody(ImmersiveEngine::cbs::RigidBody* rb)
	{
		rb->initialize(m_bodyInterface);
		m_rigidBodies.push_back(rb);
	}

	void PhysicsManager::removeRigidBody(uint32_t index)
	{
		if (index > m_rigidBodies.size()) return;

		m_bodyInterface->RemoveBody(m_rigidBodies[index]->getBodyID());
		m_rigidBodies.erase(m_rigidBodies.begin() + index);
	}

	ImmersiveEngine::cbs::RigidBody* PhysicsManager::getRigidBody(uint32_t index)
	{
		//if (index > m_rigidBodies.size()) return;

		return m_rigidBodies[index];
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

	}

	void PhysicsManager::onUpdate(float deltaTime)
	{
		refreshBodies();

		m_physicsSystem.Update(deltaTime, 1, m_tempAllocator.get(), m_jobSystem.get());
	}
}