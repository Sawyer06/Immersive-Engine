#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include<sstream>
#include<string>

#include"../Components/Component.h"

namespace ImmersiveEngine::cbs
{
    class Object
    {
        private:
            //std::unordered_map<Component, unsigned int> m_components;
        public:
            Object(std::string name);
            ~Object() = default;

            static unsigned int nextId;
            unsigned int id;

            std::string name;
            bool enabled = true;
            
            void addComponent(const Component comp, unsigned int order);
            virtual std::string toString();
    };
}
#endif
