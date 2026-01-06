#ifndef PRESENT_CLASS_H
#define PRESENT_CLASS_H

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

    class Present : public Object
    {
        private:
            void initialize() override;
        public:
            Present();
            Present(const std::string& name, std::shared_ptr<ImmersiveEngine::Rendering::Mesh> mesh);
            Present(const Present& other); // Clone constructor
            ~Present();

            Space* space;
            //Space space;
            std::shared_ptr<ImmersiveEngine::Rendering::Mesh> mesh;

            std::unique_ptr<Present> clone() const;
            //bool isCollidingWith(const Present& obj);

            //void dump();
            std::string toString() override;
    };
}
#endif
