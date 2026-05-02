#include "HitWallEffect.h"
#include <DxLib.h>
#include "Graphics/Camera.h"
#include "PosProvider.h"

namespace
{
	constexpr int graph_width = 128;//画像1枚の幅
	constexpr int graph_height = 256;//画像1枚の高さ
	constexpr float p_draw_scale = 2.5f;//描画スケール
	constexpr int p_max_anim_num = 5;//最大アニメーション数
	constexpr int p_one_anim_frame = 4;//アニメーションの待機フレーム数
}

HitWallEffect::HitWallEffect(int handle,
	const Vector2& pos) :
	Effect(pos),
	m_handle(handle)
{
	m_animation.Init(
		m_handle, 0,
		{ graph_width, graph_height },
		p_max_anim_num, p_one_anim_frame, p_draw_scale, false
	);
}

void HitWallEffect::Update()
{
	m_animation.Update();
	//アニメーションが最後まで行ったらエフェクトを消す
	if (m_animation.GetIsEnd())
	{
		m_isDead = true;
	}
}

void HitWallEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;
	const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	m_animation.Draw(drawPos, false);
}
