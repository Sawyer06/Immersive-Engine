#ifndef COMPONENT_CLASS_H
#define COMPONENT_CLASS_H

#include<sstream>
#include<string>
#include<iostream>
#include <typeindex>
#include <vector>

#define MISORDERED_ERROR std::cerr << "MISORDERED_OPERATION_ERROR component '" << typeid(this).name() << "' cannot be added. Object does not have all required components.\n"

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

			virtual std::string toString();

			virtual void onStart() { }
			virtual void onUpdate() { }
	};
}
#endif
