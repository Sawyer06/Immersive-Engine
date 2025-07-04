#include"Space.h"

namespace ImmersiveEngine::cbs
{
    Space::Space(Object* obj) : Component(obj),
        position(0, 0, 0), orientation(ImmersiveEngine::Math::Quaternion::identity),
        scale(1, 1, 1), up(0, 1, 0), m_matrix(glm::mat4(1.0f)) { }

    /// Update the position, orientation, and scale of object based on its corresponding values.
    void Space::refreshTransforms(Shader& shaderProgram)
    {
        m_matrix = glm::mat4(1.0f);

        m_matrix = glm::translate(m_matrix, glm::vec3(position.x, position.y, position.z)); // Move x, y, z

        m_matrix *= glm::mat4_cast(glm::quat(orientation.w, orientation.x, orientation.y, orientation.z));

        /*
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate x
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate y
        m_matrix = glm::rotate(m_matrix, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate z
        */

        m_matrix = glm::scale(m_matrix, glm::vec3(scale.x, scale.y, scale.z)); // Scale x, y, z

        shaderProgram.setMat4("transform", m_matrix);
    }

    void Space::translate(ImmersiveEngine::Math::Vector3 deltaPos)
    {
        position += deltaPos;
    }
    void Space::translate(ImmersiveEngine::Math::Vector2 deltaPos)
    {
        ImmersiveEngine::Math::Vector3 pos(deltaPos.x, deltaPos.y, 0);
        translate(pos);
    }

    /// NONCOMMUTATIVE : ORDER OF OPERATIONS MATTER
    void Space::rotate(float angle, ImmersiveEngine::Math::Vector3 axis)
    {
        float angleRadians = glm::radians(angle);
        ImmersiveEngine::Math::Quaternion deltaQ = ImmersiveEngine::Math::Quaternion::angleAxis(angleRadians, axis);
        
        orientation = deltaQ * orientation; // Local space.

        orientation.normalize();
    }

    void Space::rotateGlobal(float angle, ImmersiveEngine::Math::Vector3 axis)
    {
        float angleRadians = glm::radians(angle);
        ImmersiveEngine::Math::Quaternion deltaQ = ImmersiveEngine::Math::Quaternion::angleAxis(angleRadians, axis);

        orientation = orientation * deltaQ; // World space.

        orientation.normalize();
    }

    void Space::dialate(float scaleFactor)
    {
        scale *= scaleFactor;
    }
    void Space::dialate(ImmersiveEngine::Math::Vector3 scaleFactor)
    {
        scale.x *= scaleFactor.x;
        scale.y *= scaleFactor.y;
        scale.z *= scaleFactor.z;
    }
    void Space::dialate(ImmersiveEngine::Math::Vector2 scaleFactor)
    {
        ImmersiveEngine::Math::Vector3 scale(scaleFactor.x, scaleFactor.y, 0);
        dialate(scale);
    }

    std::string Space::toString()
    {
        std::ostringstream oss;
        oss << Component::toString() << "\tposition: " << position.toString() << "\n\torientation: " << orientation.toString() << "\n\tscale: " << scale.toString() << "\n";
        return oss.str();
    }
}