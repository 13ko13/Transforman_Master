#include "BulletHitEffect.h"
#include <DxLib.h>
#include "PosProvider.h"
#include "Graphics/Camera.h"

namespace
{
	constexpr int graph_width = 32;//画像1枚の幅
	constexpr int graph_height = 32;//画像1枚の高さ
	constexpr float draw_scale = 3.0f;//描画スケール
	constexpr int max_anim_num = 4;//最大アニメーション数
	constexpr int one_anim_frame = 3;//アニメーションの待機フレーム数
}

BulletHitEffect::BulletHitEffect(int handle, const Vector2& pos):
	Effect(pos),
	m_handle(handle)
{
	m_animation.Init(
		m_handle, 0,
		{ graph_width, graph_height },
		max_anim_num, one_anim_frame, draw_scale, false
	);
}

void BulletHitEffect::Update()
{
	m_animation.Update();

	if (m_animation.GetIsEnd())
	{
		m_isDead = true;
	}
}

void BulletHitEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;
	Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	m_animation.Draw(drawPos,false);
}