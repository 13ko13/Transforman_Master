#pragma once
#include "Effect.h"
#include "Graphics/Animation.h"

class HitWallEffect : public Effect
{
public:
	//固定位置エフェクトコンストラクタ
	HitWallEffect(int handle,
		const Vector2& pos);

	void Update() override;
	void Draw(const std::shared_ptr<Camera>& pCamera) override;

private:
	int m_handle;
	Animation m_animation;
};

