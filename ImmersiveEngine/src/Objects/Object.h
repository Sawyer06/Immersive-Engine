#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include<sstream>
#include<string>
#include<vector>
#include <typeindex>

#include"../Components/Component.h"
#include"../Components/Space.h"

namespace ImmersiveEngine::cbs
{
    class Object
    {   
        private:
            std::vector<std::unique_ptr<Component>> m_components;
            
            //bool hasDependencies(std::vector<std::type_index> dependencies);
        public:
            Object(std::string name);
            ~Object() = default;

            static unsigned int nextId;
            unsigned int id;

            std::string name;
            bool enabled = true;

            virtual void initialize() = 0;

            virtual std::string toString();

            /// Add a specific component to the object.
            template<typename T, typename... Args> T* addComponent(Args&&... args)
            {
                if (getComponent<T>()) // Component already added.
                {
                    std::cerr << "DUPLICATE_OPERATION_ERROR component '" << typeid(T).name() << "' cannot be added to '" << name << "': already added to this object.\n";
                    return nullptr;
                }
                /*else if (!hasDependencies(T::dependencies)) // The object the component has been added to did not have all required components.
                {
                    std::cerr << "MISORDERED_OPERATION_ERROR component '" << typeid(T).name() << "' cannot be added to '" << name << "': object does not have all required components.\n";
                    return nullptr;
                }*/
                auto newComponent = std::make_unique<T>(this, std::forward<Args>(args)...); // Object has unique ownership over component.
                T* componentPtr = newComponent.get(); // Convert to raw pointer.
                m_components.emplace_back(std::move(newComponent)); // Add to components list.
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

            /// Remove a specified component.
            template<typename T> void removeComponent()
            {
                for (auto i = m_components.begin(); i != m_components.end(); )
                {
                    if (dynamic_cast<T*>(i->get())) // Attempts to cast the component to the type that is being looked for.
                    {
                        std::cout << "'" << typeid(T).name() << "' is being removed.\n";
                        i = m_components.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
    };
}
#endif
