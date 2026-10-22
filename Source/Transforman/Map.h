#pragma once
#include <vector>
#include <memory>
#include "Collider/Rect.h"



class Stage;
class Camera;
class Player;
class Map
{
public:
	Map(std::shared_ptr<Stage> pStage);
	~Map();

	void Update();
	void Draw(Camera camera);
	bool IsCollision(const Rect hitRect, Rect& chipRect);
private:
	std::shared_ptr<Stage> m_pStage;//ステージのポインタ
	std::vector<int> m_handles;	//画像ハンドル
	bool m_isStageEnd;//ステージの終端かどうか
	int m_scrollX;//現在のスクロール度合い
	int m_startChipX;//表示開始タイル
	int m_offsetX;//スクロールの途中にチップを補正する

	enum class StageType
	{
		Stage1,
		Stage2,
		Stage3,
		Stage4,
		Stage5,
	};
	StageType m_stageType;
};

