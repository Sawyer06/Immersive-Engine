#ifndef COMPONENT_CLASS_H
#define COMPONENT_CLASS_H

#include<sstream>
#include<string>
#include<iostream>
#include <typeindex>
#include <vector>

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
			Component(Object* obj);
			Component();
			~Component() = default;

			bool enabled = true;

			static std::vector<std::type_index> dependencies;

			virtual std::string toString();

			virtual void onStart() { }
			virtual void onUpdate() { }
	};
}
#endif
