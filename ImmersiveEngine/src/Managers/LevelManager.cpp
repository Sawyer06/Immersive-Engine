#include"LevelManager.h"

namespace ImmersiveEngine
{
	/// Get a level by name.
	Level* LevelManager::getLevel(std::string name)
	{
		return nullptr;
	}

	Level* LevelManager::createNewLevel(std::string name)
	{
		std::shared_ptr<Level> newLevel = std::shared_ptr<Level>();
		m_levels.emplace(name, newLevel);
		
		return newLevel.get();
	}

	/// Get the level that is currently loaded.
	Level* LevelManager::getLoadedLevel()
	{
		if (m_loadedLevel)
		{
			std::cout << "Could not get loaded level, none has been loaded.\n";
		}

		return m_loadedLevel.get();
	}

	/// Set the currently loaded level by name.
	void LevelManager::loadLevel(std::string name)
	{
		auto levelToLoad = m_levels.find(name);
		if (levelToLoad != m_levels.end())
		{
			m_loadedLevel = levelToLoad->second;
		}
		else
		{
			std::cout << "Could not load level of name \"" << name << "\" as it does not exist.\n";
		}
	}
}