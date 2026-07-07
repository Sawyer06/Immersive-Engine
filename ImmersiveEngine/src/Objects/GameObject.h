#ifndef GAME_OBJECT_CLASS_H
#define GAME_OBJECT_CLASS_H

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"Object.h"
//#include"../Components/Space.h"
#include "../Rendering/shaderClass.h"
#include"../Rendering/Mesh.h"
#include"../Math/Math.h"
#include"../Math/Vector2.h"
#include"../Math/Vector3.h"

namespace ImmersiveEngine::cbs
{
    class Space;

    class GameObject : public Object
    {
        private:
            void initialize() override;
        public:
            GameObject();
            GameObject(const std::string& name, std::shared_ptr<ImmersiveEngine::Rendering::Mesh> mesh);
            GameObject(const GameObject& other); // Clone constructor
            ~GameObject();

            Space* space;
            
            std::shared_ptr<ImmersiveEngine::Rendering::Mesh> mesh;

            std::unique_ptr<GameObject> clone() const;

            std::string toString() override;
    };
}
#endif
