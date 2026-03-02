#pragma once
#include "Effect.h"
#include "Graphics/Animation.h"

class DashEffect : public Effect
{
public:
	/// <summary>
	/// 向きが固定されているエフェクトコンストラクタ
	/// </summary>
	/// <param name="handle">画像ハンドル</param>
	/// <param name="pos">位置</param>
	DashEffect(
		int handle,
		const Vector2& pos);
	/// <summary>
	/// 向きが変わるエフェクトコンストラクタ
	/// </summary>
	/// <param name="handle">画像ハンドル</param>
	/// <param name="pos">位置</param>
	/// <param name="isTurn">向き</param>
	DashEffect(
		int handle, 
		const Vector2& pos, 
		bool isTurn);
	void Update() override;
	void Draw(const std::shared_ptr<Camera>& pCamera) override;

private:
	int m_handle;
	bool m_isTurn;
	Animation m_animation;
};