#ifndef COLLIDERSHAPE_CLASS_H
#define COLLIDERSHAPE_CLASS_H

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include"../Math/Vector3.h"
#include"../Math/Quaternion.h"

namespace ImmersiveEngine::Physics
{
	class ColliderShape
	{
		protected:
			ColliderShape(ImmersiveEngine::Math::Vector3 positionOffset, ImmersiveEngine::Math::Quaternion orientationOffset) :
				positionOffset(positionOffset), orientationOffset(orientationOffset) { }
		public:
			virtual ~ColliderShape() = default;

			ImmersiveEngine::Math::Vector3 positionOffset = ImmersiveEngine::Math::Vector3::zero;
			ImmersiveEngine::Math::Quaternion orientationOffset = ImmersiveEngine::Math::Quaternion::identity;
	};

	class BoxShape : public ColliderShape
	{
		public:
			BoxShape(ImmersiveEngine::Math::Vector3 halfExtent,
					ImmersiveEngine::Math::Vector3 positionOffset = ImmersiveEngine::Math::Vector3::zero, 
					ImmersiveEngine::Math::Quaternion orientationOffset = ImmersiveEngine::Math::Quaternion::identity) :
				ColliderShape(positionOffset, orientationOffset), halfExtent(halfExtent) { }

			ImmersiveEngine::Math::Vector3 halfExtent;
	};

	class SphereShape : public ColliderShape
	{
		public:
			SphereShape(float radius,
				ImmersiveEngine::Math::Vector3 positionOffset = ImmersiveEngine::Math::Vector3::zero,
				ImmersiveEngine::Math::Quaternion orientationOffset = ImmersiveEngine::Math::Quaternion::identity) :
				ColliderShape(positionOffset, orientationOffset), radius(radius) {
			}

			float radius;
	};

	class CapsuleShape : public ColliderShape
	{
		public:
			CapsuleShape(float halfHeight, float radius,
				ImmersiveEngine::Math::Vector3 positionOffset = ImmersiveEngine::Math::Vector3::zero,
				ImmersiveEngine::Math::Quaternion orientationOffset = ImmersiveEngine::Math::Quaternion::identity) :
				ColliderShape(positionOffset, orientationOffset), halfHeight(halfHeight), radius(radius) { }

			float halfHeight;
			float radius;
	};

	class CylinderShape : public ColliderShape
	{
		public:
			CylinderShape(float halfHeight, float radius,
				ImmersiveEngine::Math::Vector3 positionOffset = ImmersiveEngine::Math::Vector3::zero,
				ImmersiveEngine::Math::Quaternion orientationOffset = ImmersiveEngine::Math::Quaternion::identity) :
				ColliderShape(positionOffset, orientationOffset), halfHeight(halfHeight), radius(radius) { }

			float halfHeight;
			float radius;
	};
}
#endif