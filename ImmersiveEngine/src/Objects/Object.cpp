#include"Object.h"

namespace ImmersiveEngine::cbs
{
    unsigned int Object::nextId = 0;
    Object::Object(std::string name) :
        name(name)
    {
        id = nextId;
        nextId++;
    }

    std::string Object::toString()
    {
        std::ostringstream oss;
        oss << name << " (" << id << ")";
        return oss.str();
    }
}