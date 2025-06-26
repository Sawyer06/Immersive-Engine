#include"Present.h"

namespace ImmersiveEngine::cbs
{
    Present::Present() : 
        Object("Object"), mesh(nullptr) 
    {
        initialize();
    }

    Present::Present(const std::string& name, std::shared_ptr<Mesh> mesh) :
        Object(name), mesh(mesh) 
    {
        initialize();
    }

    Present::Present(const Present& other) :
        Object(other.name), mesh(other.mesh) 
    {
        initialize();
    }
    
    Present::~Present()
    {
        //dump();
    }

    void Present::initialize()
    {
        space = addComponent<Space>();
    }

    std::unique_ptr<Present> Present::clone() const
    {
        return std::make_unique<Present>(*this);
    }

    /// AABB collision detection.
    /*bool Present::isCollidingWith(const Present& obj)
    {
        bool collisionX = space.position.x + space.scale.x / 2 > obj.space.position.x &&
            obj.space.position.x + obj.space.scale.x / 2 > space.position.x;
        bool collisionY = space.position.y + space.scale.y / 2 > obj.space.position.y &&
            obj.space.position.y + obj.space.scale.y / 2 > space.position.y;
        return collisionX && collisionY;
    }*/

    /// Memory management. Delete all used objects.
    /*void Present::dump()
    {
        m_VAO.Delete();
        m_VBO.Delete();
        //m_EBO.Delete();
        if (m_texture)
        {
            m_texture->Delete();
            delete m_texture;
            m_texture = nullptr;
        };
        std::cout << "Object " << id << " destroyed successfully\n";
    }*/

    std::string Present::toString()
    {
        std::ostringstream oss;
        oss << Object::toString() << space->toString();
        return oss.str();
    }

    /*Present::Space::Space() :
        position(0, 0, 0), orientation(0, 0, 0), pivotOffset(0, 0, 0),
        scale(1, 1, 1), up(0, 1, 0), m_matrix(glm::mat4(1.0f)) {}

    /// Update the position, orientation, and scale of object based on its corresponding values.
    void Present::Space::refreshTransforms(Shader& shaderProgram)
    {
        m_matrix = glm::mat4(1.0f);

        m_matrix = glm::translate(m_matrix, glm::vec3(position.x, position.y, position.z)); // Move x, y, z

        m_matrix = glm::translate(m_matrix, glm::vec3(pivotOffset.x, pivotOffset.y, pivotOffset.z)); // Apply rotation offset
        
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate x
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate y
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate z

        m_matrix = glm::translate(m_matrix, -glm::vec3(pivotOffset.x, pivotOffset.y, pivotOffset.z)); // Revert rotation offset

        m_matrix = glm::scale(m_matrix, glm::vec3(scale.x, scale.y, scale.z)); // Scale x, y, z

        shaderProgram.setMat4("transform", m_matrix);
    }

    void Present::Space::translate(ImmersiveEngine::Math::Vector3 deltaPos)
    {
        position += deltaPos;
    }
    void Present::Space::translate(ImmersiveEngine::Math::Vector2 deltaPos)
    {
        ImmersiveEngine::Math::Vector3 pos(deltaPos.x, deltaPos.y, 0);
        translate(pos);
    }

    void Present::Space::rotate(ImmersiveEngine::Math::Vector3 deltaAngle)
    {
        orientation += deltaAngle;
        //orientation = Engine::Math::coterminal(orientation);
    }
    void Present::Space::rotate(float deltaAngle)
    {
        ImmersiveEngine::Math::Vector3 angle(0, 0, deltaAngle);
        rotate(angle);
    }

    void Present::Space::dialate(float scaleFactor)
    {
        scale *= scaleFactor;
    }
    void Present::Space::dialate(ImmersiveEngine::Math::Vector3 scaleFactor)
    {
        scale.x *= scaleFactor.x;
        scale.y *= scaleFactor.y;
        scale.z *= scaleFactor.z;
    }
    void Present::Space::dialate(ImmersiveEngine::Math::Vector2 scaleFactor)
    {
        ImmersiveEngine::Math::Vector3 scale(scaleFactor.x, scaleFactor.y, 0);
        dialate(scale);
    }

    std::string Present::Space::toString()
    {
        std::ostringstream oss;
        oss << "\nposition: " << position.toString() << "\norientation: " << orientation.toString() << "\nscale: " << scale.toString();
        return oss.str();
    }*/
}