#pragma once
#include "SaveData.h"
#include <string>

class SaveManager
{
public:
	explicit SaveManager(std::string filePath = "progress.txt");
	bool Load(SaveData& out);
	bool Save(const SaveData& data);

	//破損時や未作成時にデフォルトデータを作成
	static SaveData DefaultData();

private:
	std::string m_filePath;

	static std::string Serialize(const SaveData& data);
	static bool Deserialize(const std::string& str, SaveData& out);
};

