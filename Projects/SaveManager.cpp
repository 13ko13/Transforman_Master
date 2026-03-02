#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

SaveManager::SaveManager(std::string filePath) :
	m_filePath(std::move(filePath))
{

}

SaveData SaveManager::DefaultData()
{
	SaveData data;
	data.version = 1;
	data.clearedStages = 0;
	data.lastPlayedStage = 1;
	data.unlockedAbilities = {};
	return data;
}

static std::vector<std::string> split(const std::string& str, char delim)
{
	std::vector<std::string> parts;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		if (!item.empty())
		{
			parts.push_back(item);
		}
	}
	return parts;
}

std::string SaveManager::Serialize(const SaveData& data)
{
	std::ostringstream oss;
	oss << data.version << "\n";
	oss << data.clearedStages << "\n";
	oss << data.lastPlayedStage << "\n";
	for (const auto& ability : data.unlockedAbilities)
	{
		oss << ability << ",";
	}
	return oss.str();
}
