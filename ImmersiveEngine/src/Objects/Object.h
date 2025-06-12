#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include<sstream>
#include<string>
#include<list>

#include"../Components/Component.h"

namespace ImmersiveEngine::cbs
{
    class Object
    {
        public:
            Object(std::string name);
            ~Object() = default;

            static unsigned int nextId;
            unsigned int id;

            std::string name;
            bool enabled = true;

            virtual std::string toString();
    };
}
#endif
