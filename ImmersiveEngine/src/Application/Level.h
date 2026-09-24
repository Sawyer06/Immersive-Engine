#ifndef LEVEL_CLASS_H
#define LEVEL_CLASS_H

#include"../Objects/Object.h"

namespace ImmersiveEngine
{
	class Level
	{
		public:
			Level() = default;
			~Level() = default;

			/*template<typename T, typename... Args> T& createObject(Args&&... args)
			{
				m_levelObjects.push_back(std::make_unique<T>(std::forward(args)...);
			}*/

			cbs::Object* findObject(const std::string name);
			cbs::Object* findObject(const unsigned int ID);

		private:
			std::vector<std::unique_ptr<cbs::Object>> m_levelObjects;
	};
}
#endif
