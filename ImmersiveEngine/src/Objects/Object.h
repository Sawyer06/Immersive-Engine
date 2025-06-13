#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include<sstream>
#include<string>
#include<list>

#include"../Components/Component.h"
#include"../Components/Space.h"

namespace ImmersiveEngine::cbs
{
    class Object
    {   
        private:
            std::list<std::unique_ptr<Component>> m_components;
        public:
            Object(std::string name);
            ~Object() = default;

            static unsigned int nextId;
            unsigned int id;

            std::string name;
            bool enabled = true;

            virtual std::string toString();

            /// Add a specific component to the object.
            template<typename T, typename... Args> T* addComponent(Args&&... args)
            {
                if (getComponent<T>()) // Component already added.
                {
                    //std::cerr << "DUPLICATE_OPERATION_ERROR component '" << typeid(T).name() << "' already added to this object.\n";
                    return nullptr;
                }
                auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
                newComponent->transferOwnership(*this);
                T* componentPtr = newComponent.get();
                m_components.emplace_back(std::move(newComponent));
                return componentPtr;
            }

            /// Return a component object of a specific type if attached.
            template<typename T> T* getComponent()
            {
                for (const auto& c : m_components)
                {
                    if (T* comp = dynamic_cast<T*>(c.get())) // Attempts to cast the component to the type that is being looked for.
                    {
                        return comp; // Component of type was found.
                    }
                }

                //std::cerr << "NULL_REFERENCE_ERROR getting a '" << typeid(T).name() << "' component that does not exist.\n";
                return nullptr; // No component of the type was found.
            }
    };
}
#endif
