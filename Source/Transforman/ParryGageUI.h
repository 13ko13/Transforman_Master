#pragma once
#include "UserInterface.h"
#include <vector>

class ParryGageUI : public UserInterface
{
public:
	ParryGageUI(int maxParryCooltime);
	~ParryGageUI();

	void Update() override;
	void Update(int nowParryGage);
	void Draw() override;

private:
	enum class HandleNomber
	{
		Gage,//ゲージ本体
		Frame,//枠
		Barrior,//バリア
		ParryBotton,//パリィのボタン
	};

	std::vector<int> m_handles;//ゲージ画像ハンドル
	int m_currentGage;//現在のゲージ
	int m_maxGage;//ゲージの最大
	int m_frame;
	int m_maxGageShakeFrame;//最大ゲージになった時に揺らしている間の時間

	bool m_isShake;//揺らしたかどうか
	bool m_isMax;//ゲージがマックスになっているかどうか

	Vector2 m_drawOffset;//描画のみずらす

private:
};