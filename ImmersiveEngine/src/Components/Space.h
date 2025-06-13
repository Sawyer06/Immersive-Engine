#ifndef SPACE_CLASS_H
#define SPACE_CLASS_H

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include"../Objects/Present.h"
#include"Component.h"
#include "../Rendering/shaderClass.h"
#include"../Math/Math.h"
#include"../Math/Vector2.h"
#include"../Math/Vector3.h"

namespace ImmersiveEngine::cbs
{
    class Object;

    class Space : public Component
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
}
#endif