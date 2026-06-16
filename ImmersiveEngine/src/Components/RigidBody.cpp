#include "RigidBody.h"

namespace ImmersiveEngine::cbs
{
	RigidBody::RigidBody(Object* obj) :
		Component(obj),
		motionType(MotionType::Static), restitution(0.0f)
	{ 
		auto owner = Component::getOwner();
		m_ownerSpace = owner->getComponent<Space>();


	}

	bool RigidBody::isActive()
	{
		if (m_bodyInterface == nullptr) return false;

		return m_bodyInterface->IsActive(m_ID);
	}
	JPH::BodyID RigidBody::getBodyID()
	{
		if (m_bodyInterface == nullptr) return JPH::BodyID();

		return m_ID;
	}

	ImmersiveEngine::Math::Vector3 RigidBody::getLinearVelocity()
	{
		if (m_bodyInterface == nullptr) return ImmersiveEngine::Math::Vector3();

		JPH::Vec3 velocity = m_bodyInterface->GetLinearVelocity(m_ID);

		return ImmersiveEngine::Math::Vector3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
	}
	void RigidBody::setLinearVelocity(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr) return;

		m_bodyInterface->SetLinearVelocity(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	ImmersiveEngine::Math::Vector3 RigidBody::getAngularVelocity()
	{
		if (m_bodyInterface == nullptr) return ImmersiveEngine::Math::Vector3();

		JPH::Vec3 velocity = m_bodyInterface->GetAngularVelocity(m_ID);

		return ImmersiveEngine::Math::Vector3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
	}
	void RigidBody::setAngularVelocity(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr) return;

		m_bodyInterface->SetAngularVelocity(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	void RigidBody::addForce(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr) return;

		m_bodyInterface->AddForce(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}
	void RigidBody::addImpulse(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr) return;

		m_bodyInterface->AddImpulse(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}
	void RigidBody::addTorque(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr) return;

		m_bodyInterface->AddTorque(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	void RigidBody::refreshRigidBody()
	{

	}

	std::string RigidBody::toString()
	{
		std::ostringstream oss;
		oss << Component::toString();
		return oss.str();
	}
}