#pragma once
#include "../Collider/Rect.h"
#include <vector>

class Stage
{
public:

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// ファイルデータの読み込み
	/// </summary>
	/// <param name="stageNom">どこのステージか</param>
	void Load(int stageNom);
	/// <summary>
	/// マップのサイズを取得する
	/// </summary>
	/// <returns>マップのデータサイズ</returns>
	Size GetMapSize() const;
	//unsigned int →　符号なしのint型(この場合は8ビット)
	uint8_t GetData(int xidx, int yidx);
	/// <summary>
	/// FMFから読み込んだマップデータ全体を参照で返す
	/// </summary>
	/// <returns>マップデータ全体</returns>
	const std::vector<uint8_t>& GetAllData() const;
private:
	//データサイズ
	Size m_dataSize;
	//データ本体(8ビット)
	std::vector<uint8_t> m_data;
};

