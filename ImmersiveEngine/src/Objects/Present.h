#ifndef PRESENT_CLASS_H
#define PRESENT_CLASS_H

#include<string>
#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Rendering/shaderClass.h"
#include"../Rendering/VAO.h"
#include"../Rendering/VBO.h"
#include"../Rendering/EBO.h"
#include"../Rendering/Texture.h"
#include"../Math/Vector2.h"
#include"../Math/Vector3.h"

class Present
{
    private:
        class Space
        {
            private:
                glm::mat4 m_matrix;
            public:
                Space();
                
                Engine::Math::Vector3 position;
                Engine::Math::Vector3 orientation;
                Engine::Math::Vector3 scale;

                void updateTransforms(Shader shaderProgram);

                void translate(Engine::Math::Vector3 deltaPos);
                void translate(Engine::Math::Vector2 deltaPos);
                
                void rotate(Engine::Math::Vector3 deltaAngle);
                void rotate(float deltaAngle);
                
                void dialate(float scaleFactor);
                void dialate(Engine::Math::Vector3 scaleFactor);
                void dialate(Engine::Math::Vector2 scaleFactor);

                std::string toString();
        };
        VAO m_VAO;
        VBO m_VBO;
        EBO m_EBO;
        Texture* m_texture;
        int m_indexCount;
	public:
        Present();
        Present(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize);
        ~Present();

        static unsigned int nextId;
        unsigned int id;

        std::string name;
        
        Space space;

        void reInitialize(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize);
        void draw(Shader& shaderProgram);
        void setTexture(const char* filePath, GLenum format);

        void dump();

        std::string toString();
};
#endif
