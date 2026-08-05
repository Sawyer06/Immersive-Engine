#include"Level.h"

namespace ImmersiveEngine
{
	cbs::Object* ImmersiveEngine::Level::findObject(const std::string name)
	{
		auto obj = std::find_if(levelObjects.begin(), levelObjects.end(),
			[&name](const cbs::Object& o) { return o.name == name; });

		if (obj == levelObjects.end())
		{
			std::cout << "Could not find object of name " << name;
			return nullptr;
		}

		return &(*obj);
	}
}