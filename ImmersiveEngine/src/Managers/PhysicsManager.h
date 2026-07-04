#ifndef PHYSICS_MANAGER_CLASS_H
#define PHYSICS_MANAGER_CLASS_H

#include<iostream>
#include <cstdarg>
#include <map>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include"Manager.h"
#include"../Math/Vector3.h"
#include"../Objects/Present.h"
#include"../Components/RigidBody.h"

namespace ImmersiveEngine::cbs
{
	static void IETrace(const char* inFMT, ...)
	{
		// Format the message
		va_list list;
		va_start(list, inFMT);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		std::cout << buffer << std::endl;
	}

#ifdef JPH_ENABLE_ASSERTS

	static bool IEAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
	{
		std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

		return true;
	};

#endif // JPH_ENABLE_ASSERTS

	namespace Layers
	{
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
	}

	class IEObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
	{
		public:
			virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
			{
				switch (inObject1)
				{
					case Layers::NON_MOVING:
						return inObject2 == Layers::MOVING;
					case Layers::MOVING:
						return true;
					default:
						return false;
				}
			}
	};

	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr uint32_t NUM_LAYERS(2);
	}

	class IEBPLayerInterface final : public JPH::BroadPhaseLayerInterface
	{
		public:
			IEBPLayerInterface()
			{
				m_objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
				m_objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
			}

			virtual uint32_t GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}
			virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
			{
				return m_objectToBroadPhase[inLayer];
			}
	#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
			virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
			{
				switch ((JPH::BroadPhaseLayer::Type)inLayer)
				{
					case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
						return "NON_MOVING";
					case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
						return "MOVING";
					default:
						JPH_ASSERT(false); return "INVALID";
				}
			}
	#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
		private:
			JPH::BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
		};

	class IEObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
	{
		public:
			virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
			{
				switch (inLayer1)
				{
					case Layers::NON_MOVING:
						return inLayer2 == BroadPhaseLayers::MOVING;
					case Layers::MOVING:
						return true;
					default:
						return false;
				}
			}
	};

	class IEContactListener : public JPH::ContactListener
	{
		public:
			virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollectionResult) override
			{
				return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
			}

			virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
			{
				Present* obj1 = reinterpret_cast<Present*>(inBody1.GetUserData());
				Present* obj2 = reinterpret_cast<Present*>(inBody2.GetUserData());

				if (!obj1 || !obj2)
				{
					std::cerr << "PHYSICS_ERROR Could not handle contact added. Unable to retrieve user data.\n";
					return;
				}

				ImmersiveEngine::Math::Vector3 baseOffset(inManifold.mBaseOffset.GetX(), inManifold.mBaseOffset.GetY(), inManifold.mBaseOffset.GetZ());

				// Create contact info for inBody1 and call its contact handler functions.
				std::vector<ImmersiveEngine::Math::Vector3> relativeContactPointsPosition1 = { };
				std::vector<ImmersiveEngine::Math::Vector3> worldContactPointsPosition1 = { };
				for (int i = 0; i < inManifold.mRelativeContactPointsOn1.size(); i++)
				{
					JPH::Vec3 rContactPoint = inManifold.mRelativeContactPointsOn1[i];
					relativeContactPointsPosition1.emplace_back(ImmersiveEngine::Math::Vector3(rContactPoint.GetX(), rContactPoint.GetY(), rContactPoint.GetZ()));
					
					JPH::Vec3 wContactPoint = inManifold.GetWorldSpaceContactPointOn1(i);
					worldContactPointsPosition1.emplace_back(ImmersiveEngine::Math::Vector3(wContactPoint.GetX(), wContactPoint.GetY(), wContactPoint.GetZ()));
				}
				
				ImmersiveEngine::Physics::ContactInfo contact1 = { obj2, baseOffset, relativeContactPointsPosition1, worldContactPointsPosition1, inManifold.mPenetrationDepth };
				
				std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers1 = GetContactHandlers(obj1);
				for (auto handler : handlers1)
					handler->OnContactBegan(contact1);

				// Create contact info for inBody2 and call its contact handler functions.
				std::vector<ImmersiveEngine::Math::Vector3> relativeContactPointsPosition2 = { };
				std::vector<ImmersiveEngine::Math::Vector3> worldContactPointsPosition2 = { };
				for (int i = 0; i < inManifold.mRelativeContactPointsOn2.size(); i++)
				{
					JPH::Vec3 rContactPoint = inManifold.mRelativeContactPointsOn2[i];
					relativeContactPointsPosition2.emplace_back(ImmersiveEngine::Math::Vector3(rContactPoint.GetX(), rContactPoint.GetY(), rContactPoint.GetZ()));

					JPH::Vec3 wContactPoint = inManifold.GetWorldSpaceContactPointOn2(i);
					worldContactPointsPosition2.emplace_back(ImmersiveEngine::Math::Vector3(wContactPoint.GetX(), wContactPoint.GetY(), wContactPoint.GetZ()));
				}

				ImmersiveEngine::Physics::ContactInfo contact2 = { obj1, baseOffset, relativeContactPointsPosition2, worldContactPointsPosition2, inManifold.mPenetrationDepth };

				std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers2 = GetContactHandlers(obj2);
				for (auto handler : handlers2)
					handler->OnContactBegan(contact2);

				m_contactsCache.insert({ std::pair(inBody1.GetID(), inBody2.GetID()), std::pair(&inBody1, &inBody2) }); // Add the contact to the cache.
			}

			virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
			{
				Present* obj1 = reinterpret_cast<Present*>(inBody1.GetUserData());
				Present* obj2 = reinterpret_cast<Present*>(inBody2.GetUserData());

				if (!obj1 || !obj2)
				{
					std::cerr << "PHYSICS_ERROR Could not handle contact added. Unable to retrieve user data.\n";
					return;
				}

				ImmersiveEngine::Math::Vector3 baseOffset(inManifold.mBaseOffset.GetX(), inManifold.mBaseOffset.GetY(), inManifold.mBaseOffset.GetZ());

				// Create contact info for inBody1 and call its contact handler functions.
				std::vector<ImmersiveEngine::Math::Vector3> relativeContactPointsPosition1 = { };
				std::vector<ImmersiveEngine::Math::Vector3> worldContactPointsPosition1 = { };
				for (int i = 0; i < inManifold.mRelativeContactPointsOn1.size(); i++)
				{
					JPH::Vec3 rContactPoint = inManifold.mRelativeContactPointsOn1[i];
					relativeContactPointsPosition1.emplace_back(ImmersiveEngine::Math::Vector3(rContactPoint.GetX(), rContactPoint.GetY(), rContactPoint.GetZ()));

					JPH::Vec3 wContactPoint = inManifold.GetWorldSpaceContactPointOn1(i);
					worldContactPointsPosition1.emplace_back(ImmersiveEngine::Math::Vector3(wContactPoint.GetX(), wContactPoint.GetY(), wContactPoint.GetZ()));
				}

				ImmersiveEngine::Physics::ContactInfo contact1 = { obj2, baseOffset, relativeContactPointsPosition1, worldContactPointsPosition1, inManifold.mPenetrationDepth };

				std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers1 = GetContactHandlers(obj1);
				for (auto handler : handlers1)
					handler->OnContactPersisted(contact1);

				// Create contact info for inBody2 and call its contact handler functions.
				std::vector<ImmersiveEngine::Math::Vector3> relativeContactPointsPosition2 = { };
				std::vector<ImmersiveEngine::Math::Vector3> worldContactPointsPosition2 = { };
				for (int i = 0; i < inManifold.mRelativeContactPointsOn2.size(); i++)
				{
					JPH::Vec3 rContactPoint = inManifold.mRelativeContactPointsOn2[i];
					relativeContactPointsPosition2.emplace_back(ImmersiveEngine::Math::Vector3(rContactPoint.GetX(), rContactPoint.GetY(), rContactPoint.GetZ()));

					JPH::Vec3 wContactPoint = inManifold.GetWorldSpaceContactPointOn2(i);
					worldContactPointsPosition2.emplace_back(ImmersiveEngine::Math::Vector3(wContactPoint.GetX(), wContactPoint.GetY(), wContactPoint.GetZ()));
				}

				ImmersiveEngine::Physics::ContactInfo contact2 = { obj1, baseOffset, relativeContactPointsPosition2, worldContactPointsPosition2, inManifold.mPenetrationDepth };

				std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers2 = GetContactHandlers(obj2);
				for (auto handler : handlers2)
					handler->OnContactPersisted(contact2);

			}

			virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
			{
				auto contactToRemove = m_contactsCache.find(std::pair(inSubShapePair.GetBody1ID(), inSubShapePair.GetBody2ID()));
				if (contactToRemove != m_contactsCache.end()) // Make sure it is in the cache.
				{
					auto body1 = contactToRemove->second.first;
					auto body2 = contactToRemove->second.second;

					Present* obj1 = reinterpret_cast<Present*>(body1->GetUserData());
					Present* obj2 = reinterpret_cast<Present*>(body2->GetUserData());

					std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers1 = GetContactHandlers(obj1);
					for(auto handler : handlers1)
						handler->OnContactRemoved();

					std::vector<ImmersiveEngine::Physics::ContactHandler*> handlers2 = GetContactHandlers(obj2);
					for (auto handler : handlers2)
						handler->OnContactRemoved();

					m_contactsCache.erase(contactToRemove); // Remove the contact from the cache once used.
				}
				else
				{
					std::cerr << "PHYSICS_ERROR Contact to remove was not found in the cache.\n";
				}
			}

		private:
			std::vector<ImmersiveEngine::Physics::ContactHandler*> GetContactHandlers(Present* obj)
			{
				std::vector< ImmersiveEngine::Physics::ContactHandler*> contactHandlers;
				for (const auto& comp : obj->getAllComponents())
				{
					auto* handler = dynamic_cast<ImmersiveEngine::Physics::ContactHandler*>(comp.get());
					if (handler != nullptr) // Component also has a contact handler.
					{
						contactHandlers.push_back(handler);
					}
				}
				
				return contactHandlers;
			}

			// Necessary to keep track of added contacts as they are inaccesible upon OnContactRemoved()
			std::map<std::pair<JPH::BodyID, JPH::BodyID>, std::pair<const JPH::Body*, const JPH::Body*>> m_contactsCache = { };
	};

	class IEBodyActivationListener : public JPH::BodyActivationListener
	{
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			//std::cout << "A body got activated" << std::endl;
		}

		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			//std::cout << "A body went to sleep" << std::endl;
		}
	};

	class PhysicsManager : public Manager<PhysicsManager>
	{
		friend class Manager<PhysicsManager>;

		private:
			PhysicsManager() = default;
			~PhysicsManager() = default;

			JPH::PhysicsSystem m_physicsSystem;
			JPH::BodyInterface* m_bodyInterface;

			std::vector<RigidBody*> m_rigidBodies;
			
			std::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
			std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;
			IEBPLayerInterface m_broadPhaseLayerInterface;
			IEObjectVsBroadPhaseLayerFilter m_objectVsBroadphaseLayerFilter;
			IEObjectLayerPairFilter m_objectVsObjectLayerFilter;
			IEBodyActivationListener m_bodyActivationListener;
			IEContactListener m_contactListener;
		public:
			uint32_t maxBodies = 1024;
			uint32_t numBodyMutexes = 0;
			uint32_t maxBodyPairs = 1024;
			uint32_t maxContactConstraints = 1024;

			void initialize();
			void addRigidBody(RigidBody* rb);
			void removeRigidBody(uint32_t index);
			RigidBody* getRigidBody(uint32_t index);

			void refreshBodies(float deltaTime);
	};
}
#endif
