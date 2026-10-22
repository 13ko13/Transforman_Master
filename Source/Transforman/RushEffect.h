#pragma once
#include "Effect.h"
#include "Graphics/Animation.h"

class RushEffect : public Effect
{
public:
	//関数オーバーロードにより2つのコンストラクタを用意
	//追従エフェクトコンストラクタ
	RushEffect(int handle,
		std::shared_ptr<PosProvider> target,
		const Vector2& offset);

	//固定位置エフェクトコンストラクタ
	RushEffect(int handle,
		const Vector2& pos);

	void Update() override;
	void Draw(const std::shared_ptr<Camera>& pCamera) override;

private:
	int m_handle;
	Animation m_animation;
};