#pragma once
#include <string>
#include <vector>

struct SaveData
{
	int version = 1;
	int clearedStages = 0;
	int lastPlayedStage = 1;
	std::vector<std::string> unlockedAbilities;

	int NextStageToLoad(int maxStage) const
	{
		return std::min(clearedStages + 1, maxStage);
	}
};