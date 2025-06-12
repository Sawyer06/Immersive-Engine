#ifndef COMPONENT_CLASS_H
#define COMPONENT_CLASS_H

#include<sstream>
#include<string>
#include<iostream>

namespace ImmersiveEngine::cbs
{
	class Object;

	class Component
	{
		private:
			std::weak_ptr<Object> m_owner;
		protected:
			std::shared_ptr<Object> getOwner() const;
			void transferOwnership(const std::shared_ptr<Object> newObj);
		public:
			Component();
			Component(const std::shared_ptr<Object> obj);
			~Component() = default;

			bool enabled = true;

			virtual void onStart() { }
			virtual void onUpdate() { }
	};
}
#endif
