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

    std::string Present::toString()
    {
        std::ostringstream oss;
        oss << Object::toString() << space->toString();
        return oss.str();
    }
}