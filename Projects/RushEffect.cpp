#include "RushEffect.h"
#include <DxLib.h>
#include "PosProvider.h"
#include "Graphics/Camera.h"

namespace
{
	constexpr int graph_width = 128;//画像1枚の幅
	constexpr int graph_height = 128;//画像1枚の高さ
	constexpr float p_draw_scale = 1.5f;//描画スケール
	constexpr int p_max_anim_num = 4;//最大アニメーション数
	constexpr int p_one_anim_frame = 4;//アニメーションの待機フレーム数
}

RushEffect::RushEffect(
	int handle,
	std::shared_ptr<PosProvider> target,
	const Vector2& offset):
	Effect(Vector2 {}), m_handle(handle)
{
	m_target = std::move(target);
	m_offset = offset;
	m_isFollow = true;//追従エフェクトフラグを立てる

	m_animation.Init(
		m_handle, 0,
		{ graph_width, graph_height },
		p_max_anim_num, p_one_anim_frame, p_draw_scale, true
	);
}

RushEffect::RushEffect(int handle, const Vector2& pos):
	Effect(pos),
	m_handle(handle)
{
	//固定位置エフェクトなので追従フラグを立てない
	m_isFollow = false;

	m_animation.Init(
				m_handle, 0,
				{ graph_width, graph_height },
		p_max_anim_num, p_one_anim_frame, p_draw_scale, true
				);
}

void RushEffect::Update()
{
	//追従エフェクトの場合は位置を更新
	if (m_isFollow)
	{
		if(auto target = m_target.lock())
		{
			m_pos = target->GetPos() + m_offset;
		}
		else
		{
			//ターゲットが無効ならエフェクトも消す
			m_isDead = true;
			return;
		}
	}

	m_animation.Update();

	if (m_animation.GetIsEnd())
	{
		m_isDead = true;
	}
}

void RushEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;
	const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	m_animation.Draw(drawPos,false);
}