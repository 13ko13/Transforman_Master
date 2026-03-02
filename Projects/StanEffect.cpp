#include "StanEffect.h"
#include "Graphics/Camera.h"

namespace
{
	constexpr int graph_width = 64;//画像1枚の幅
	constexpr int graph_height = 256;//画像1枚の高さ
	constexpr float draw_scale = 1.5f;//描画スケール
	constexpr int max_anim_num = 5;//最大アニメーション数
	constexpr int one_anim_frame = 4;//アニメーションの待機フレーム数
}

StanEffect::StanEffect(int handle, const Vector2& pos) :
	Effect(pos),
	m_handle(handle)
{
	m_animation.Init(
		m_handle, 0,
		{ graph_width, graph_height },
		max_anim_num, one_anim_frame,
		draw_scale, false);
}

void StanEffect::Update()
{
	m_animation.Update();
	//アニメーションが最後まで行ったらエフェクトを消す
	if (m_animation.GetIsEnd())
	{
		m_isDead = true;
	}
}

void StanEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;
	const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	m_animation.Draw(drawPos, false);
}
