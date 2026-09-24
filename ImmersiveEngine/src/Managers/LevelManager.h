#ifndef LEVEL_MANAGER_CLASS_H
#define LEVEL_MANAGER_CLASS_H

#include"../Application/Level.h"

namespace ImmersiveEngine
{
	class LevelManager
	{
		public:
			LevelManager() = default;
			~LevelManager() = default;

			Level* getLevel(std::string name);
			Level* createNewLevel(std::string name);

			Level* getLoadedLevel();
			void loadLevel(std::string name);

		private:
			std::unordered_map<std::string, std::shared_ptr<Level>> m_levels;

			std::shared_ptr<Level> m_loadedLevel;
	};
}
#endif
