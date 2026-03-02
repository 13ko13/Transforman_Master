#pragma once
#include <vector>
#include "UserInterface.h"

class EnemyHpBar : public UserInterface
{
public:
	EnemyHpBar(int maxHP);
	~EnemyHpBar();
	
	void Update(int currentHP);
	void Update() override;
	void Draw() override;
private:
	enum class HandleNomber
	{
		Frame,//枠
		Heart,//ハート
		Normal,//通常HP
		Pinch,//ピンチ時
		Empty,//空HP
	};

	inline void DrawRectRotaGraphLT(
		int x, int y,//描画先の左上座標
		int srcX, int srcY,//切り取り元の左上
		int srcW, int srcH,//切り取りサイズ
		double scale,//拡大率
		int handle, bool trans);

	int m_maxHP;
	int m_currentHP;
	std::vector<int> m_handles;//ヒットポイントの画像ハンドル
};

