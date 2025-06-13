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
            Space* m_space;

            void initialize();
            /*class Space
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
            };*/
        public:
            Present();
            Present(const std::string& name, std::shared_ptr<Mesh> mesh);
            Present(const Present& other); // Clone constructor
            ~Present();

            Space* space();
            //Space space;
            std::shared_ptr<Mesh> mesh;

            std::unique_ptr<Present> clone() const;
            //bool isCollidingWith(const Present& obj);

            //void dump();
            std::string toString() override;
    };
}
#endif
