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
			Object* m_owner;
		protected:
			Object* getOwner() const;
		public:
			Component();
			Component(Object& obj);
			~Component() = default;

			void transferOwnership(Object& obj);
			bool enabled = true;

			virtual void onStart() { }
			virtual void onUpdate() { }
	};
}
#endif
