#include "RigidBody.h"

namespace ImmersiveEngine::cbs
{
	RigidBody::RigidBody(Object* obj, std::shared_ptr<ImmersiveEngine::Physics::ColliderShape> colliderShape, MotionType motionType) :
		Component(obj), m_colliderShape(colliderShape), motionType(motionType)
	{ 
		auto owner = Component::getOwner();
		m_ownerSpace = owner->getComponent<Space>();
	}

	/// Initialize function only to be called by PhysicsManager. RigidBody cannot be used if this has not been done.
	void RigidBody::initialize(JPH::BodyInterface* bodyInterface)
	{
		m_bodyInterface = bodyInterface;

		// Check which type collider shape is.
		JPH::Shape* bodyShape = nullptr;
		if (ImmersiveEngine::Physics::BoxShape* collider = dynamic_cast<ImmersiveEngine::Physics::BoxShape*>(m_colliderShape.get()))
		{
			bodyShape = new JPH::BoxShape({ collider->halfExtent.x * m_ownerSpace->scale.x, 
				collider->halfExtent.y * m_ownerSpace->scale.y, 
				collider->halfExtent.z * m_ownerSpace->scale.z }); // Scale collider by object's scale.
		}
		else if (ImmersiveEngine::Physics::SphereShape* collider = dynamic_cast<ImmersiveEngine::Physics::SphereShape*>(m_colliderShape.get()))
		{
			bodyShape = new JPH::SphereShape(collider->radius * (m_ownerSpace->scale.x + m_ownerSpace->scale.z) / 2); // Get average of X and Z of object's scale to get radius.
		}
		else if (ImmersiveEngine::Physics::CapsuleShape* collider = dynamic_cast<ImmersiveEngine::Physics::CapsuleShape*>(m_colliderShape.get()))
		{
			bodyShape = new JPH::CapsuleShape(collider->halfHeight * m_ownerSpace->scale.y, 
				collider->radius * (m_ownerSpace->scale.x + m_ownerSpace->scale.z) / 2); // Get average of X and Z of object's scale to get radius.
		}
		else if (ImmersiveEngine::Physics::CylinderShape* collider = dynamic_cast<ImmersiveEngine::Physics::CylinderShape*>(m_colliderShape.get()))
		{
			bodyShape = new JPH::CylinderShape(collider->halfHeight * m_ownerSpace->scale.x, 
				collider->radius * (m_ownerSpace->scale.x + m_ownerSpace->scale.z) / 2); // Get average of X and Z of object's scale to get radius.
		}

		if (!bodyShape) return;

		JPH::EMotionType bodyType;
		switch (motionType)
		{
			case MotionType::Static:
				bodyType = JPH::EMotionType::Static;
				break;
			case MotionType::Kinematic:
				bodyType = JPH::EMotionType::Kinematic;
				break;
			default:
				bodyType = JPH::EMotionType::Dynamic;
		}

		JPH::RVec3 bodyPosition(m_ownerSpace->position.x + m_colliderShape->positionOffset.x,
			m_ownerSpace->position.y + m_colliderShape->positionOffset.y, 
			m_ownerSpace->position.z + m_colliderShape->positionOffset.z);

		ImmersiveEngine::Math::Quaternion rot = m_colliderShape->orientationOffset * m_ownerSpace->orientation;

		JPH::Quat bodyOrientation(rot.x, rot.y, rot.z, rot.w);

		m_bodyCreationSettings = JPH::BodyCreationSettings(bodyShape, bodyPosition, bodyOrientation, bodyType, 1);

		m_ID = bodyInterface->CreateAndAddBody(m_bodyCreationSettings, autoActivate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
		bodyInterface->SetUserData(m_ID, reinterpret_cast<uintptr_t>(getOwner()));

		m_bodyInterface->SetIsSensor(m_ID, isSensor);
		m_bodyInterface->SetRestitution(m_ID, restitution);
	}

	bool RigidBody::isActive()
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return false;
		
		return m_bodyInterface->IsActive(m_ID);
	}
	JPH::BodyID RigidBody::getBodyID()
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return JPH::BodyID();

		return m_ID;
	}

	ImmersiveEngine::Math::Vector3 RigidBody::getLinearVelocity()
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return ImmersiveEngine::Math::Vector3();

		JPH::Vec3 velocity = m_bodyInterface->GetLinearVelocity(m_ID);

		return ImmersiveEngine::Math::Vector3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
	}
	void RigidBody::setLinearVelocity(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->SetLinearVelocity(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	ImmersiveEngine::Math::Vector3 RigidBody::getAngularVelocity()
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return ImmersiveEngine::Math::Vector3();

		JPH::Vec3 velocity = m_bodyInterface->GetAngularVelocity(m_ID);

		return ImmersiveEngine::Math::Vector3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
	}
	void RigidBody::setAngularVelocity(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->SetAngularVelocity(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	void RigidBody::addForce(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->AddForce(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}
	void RigidBody::addImpulse(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->AddImpulse(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}
	void RigidBody::addTorque(ImmersiveEngine::Math::Vector3 amnt)
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->AddTorque(m_ID, JPH::Vec3(amnt.x, amnt.y, amnt.z));
	}

	/// To be called by the physics manager.
	void RigidBody::refreshRigidBody()
	{
		JPH::Vec3 position = m_bodyInterface->GetCenterOfMassPosition(m_ID);
		JPH::Quat orientation = m_bodyInterface->GetRotation(m_ID);

		m_ownerSpace->position = ImmersiveEngine::Math::Vector3(position.GetX(), position.GetY(), position.GetZ()) - m_colliderShape->positionOffset;
		m_ownerSpace->orientation = m_colliderShape->orientationOffset * 
			ImmersiveEngine::Math::Quaternion(orientation.GetW(), orientation.GetX(), orientation.GetY(), orientation.GetZ()); // Rotate locally.
	}

	void RigidBody::dump()
	{
		if (m_bodyInterface == nullptr || m_ID.IsInvalid()) return;

		m_bodyInterface->RemoveBody(m_ID);
		m_bodyInterface->DestroyBody(m_ID);
	}

	std::string RigidBody::toString()
	{
		std::ostringstream oss;
		oss << Component::toString() <<
			"\tIs Sensor: " << std::boolalpha << isSensor <<
			"\n\tMotion Type: " << motionType <<
			"\n\tRestitution: " << restitution <<
			"\n\tAuto Activate: " << std::boolalpha << autoActivate <<
			"\n\tID valid: " << std::boolalpha << !m_ID.IsInvalid() << "\n";
		return oss.str();
	}
}