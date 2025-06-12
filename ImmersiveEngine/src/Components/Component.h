#ifndef COMPONENT_CLASS_H
#define COMPONENT_CLASS_H

#include<sstream>
#include<string>
#include<iostream>

namespace ImmersiveEngine::cbs
{
	class Object;
	class Present;

	class Component
	{
		private:
			std::weak_ptr<Present> m_owner;
		protected:
			std::shared_ptr<Present> getOwner() const;
			void transferOwnership(const std::shared_ptr<Present> newObj);
		public:
			Component();
			Component(const std::shared_ptr<Present> obj);
			~Component() = default;

			bool enabled = true;

			virtual void onStart() { }
			virtual void onUpdate() { }
	};
}
#endif
