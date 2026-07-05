#include"GameObject.h"

namespace ImmersiveEngine::cbs
{
    GameObject::GameObject() : 
        Object("Object"), mesh(nullptr) 
    {
        initialize();
    }

    GameObject::GameObject(const std::string& name, std::shared_ptr<ImmersiveEngine::Rendering::Mesh> mesh) :
        Object(name), mesh(mesh) 
    {
        initialize();
    }

    GameObject::GameObject(const GameObject& other) :
        Object(other.name), mesh(other.mesh) 
    {
        initialize();
    }
    
    GameObject::~GameObject()
    {
        //dump();
    }

    void GameObject::initialize()
    {
        space = addComponent<Space>();
    }

    std::unique_ptr<GameObject> GameObject::clone() const
    {
        return std::make_unique<GameObject>(*this);
    }

    std::string GameObject::toString()
    {
        std::ostringstream oss;
        oss << Object::toString() << space->toString();
        return oss.str();
    }
}