#ifndef LEVEL_CLASS_H
#define LEVEL_CLASS_H

#include"../Objects/Object.h"

namespace ImmersiveEngine
{
	class Level
	{
		public:
			Level();
			~Level();

			std::vector<cbs::Object> levelObjects;

			cbs::Object* findObject(const std::string name);

	};
}
#endif
