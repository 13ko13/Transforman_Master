#include "DashEffect.h"
#include <DxLib.h>
#include "PosProvider.h"
#include "Graphics/Camera.h"

namespace
{
		constexpr int graph_width = 64;//画像1枚の幅
		constexpr int graph_height = 64;//画像1枚の高さ
		constexpr float p_draw_scale = 1.5f;//描画スケール
		constexpr int p_max_anim_num = 16;//最大アニメーション数
		constexpr int p_one_anim_frame = 3;//アニメーションの待機フレーム数
		constexpr int p_src_y = 11;//画像の縦切り取り位置
}

DashEffect::DashEffect(int handle, const Vector2& pos):
	Effect(pos),
	m_handle(handle),
	m_isTurn(false)
{
	//固定位置エフェクトなので追従フラグを立てない
	m_isFollow = false;
	m_animation.Init(
				m_handle, p_src_y,
				{ graph_width, graph_height },
				p_max_anim_num, p_one_anim_frame, p_draw_scale, false
	);
}

DashEffect::DashEffect(int handle, const Vector2& pos, bool isTurn):
	Effect(pos),
	m_handle(handle),
	m_isTurn(isTurn)
{
	//固定位置エフェクトなので追従フラグを立てない
	m_isFollow = false;

	m_animation.Init(
				m_handle, p_src_y,
				{ graph_width, graph_height },
				p_max_anim_num, p_one_anim_frame, p_draw_scale, false
				);
}

void DashEffect::Update()
{
	m_animation.Update();

	if (m_animation.GetIsEnd())
	{
		m_isDead = true;
	}
}

void DashEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;
	Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	m_animation.SetOffset({ 0.0f,0.0f });
	m_animation.Draw(drawPos,m_isTurn);
}
