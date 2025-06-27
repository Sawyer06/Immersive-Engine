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

    /*/// Check if the current object contains the required subset for the one being added (ex. camera component requires a space component).
    bool Object::hasDependencies(std::vector<std::type_index> dependencies)
    {
        for (const auto& comp : m_components)
        {
            bool canAdd = false;
            for (const std::type_index index : dependencies)
            {
                if (index == typeid(*comp))
                {
                    canAdd = true;
                    break;
                }
            }
            if (!canAdd)
            {
                return false;
            }
        }
        return true;
    }*/

    std::string Object::toString()
    {
        std::ostringstream oss;
        oss << name << " (" << id << ")";
        return oss.str();
    }
}