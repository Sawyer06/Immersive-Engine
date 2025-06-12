#ifndef PRESENT_CLASS_H
#define PRESENT_CLASS_H

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"Object.h"
#include "../Rendering/shaderClass.h"
#include"../Rendering/Mesh.h"
#include"../Math/Math.h"
#include"../Math/Vector2.h"
#include"../Math/Vector3.h"

namespace ImmersiveEngine::cbs
{
    class Present : public Object
    {
        private:
            std::list<std::unique_ptr<Component>> m_components;

            class Space
            {
                private:
                    glm::mat4 m_matrix;
                public:
                    Space();

                    ImmersiveEngine::Math::Vector3 position;
                    ImmersiveEngine::Math::Vector3 orientation;
                    ImmersiveEngine::Math::Vector3 pivotOffset;
                    ImmersiveEngine::Math::Vector3 scale;
                    ImmersiveEngine::Math::Vector3 up;

                    void refreshTransforms(Shader& shaderProgram);

                    void translate(ImmersiveEngine::Math::Vector3 deltaPos);
                    void translate(ImmersiveEngine::Math::Vector2 deltaPos);

                    void rotate(ImmersiveEngine::Math::Vector3 deltaAngle);
                    void rotate(float deltaAngle);

                    void dialate(float scaleFactor);
                    void dialate(ImmersiveEngine::Math::Vector3 scaleFactor);
                    void dialate(ImmersiveEngine::Math::Vector2 scaleFactor);

                    std::string toString();
            };
        public:
            Present();
            Present(const std::string& name, std::shared_ptr<Mesh> mesh);
            Present(const Present& other); // Clone constructor
            ~Present();

            Space space;
            std::shared_ptr<Mesh> mesh;

            std::unique_ptr<Present> clone() const;
            bool isCollidingWith(const Present& obj);

            //void dump();
            std::string toString() override;

            /// Add a specific component to the object.
            template<typename T, typename... Args> T* addComponent(Args&&... args)
            {
                if (getComponent<T>()) // Component already added.
                {
                    std::cerr << "DUPLICATE_OPERATION_ERROR component '" << typeid(T).name() << "' already added to this object.\n";
                    return nullptr;
                }
                auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
                T* componentPtr = newComponent.get();
                m_components.push_back(std::move(newComponent));
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

                std::cerr << "NULL_REFERENCE_ERROR getting a '" << typeid(T).name() << "' component that does not exist.\n";
                return nullptr; // No component of the type was found.
            }
    };
}
#endif
