#include "DeathEffect.h"
#include <DxLib.h>
#include "PosProvider.h"
#include "Graphics/Camera.h"

namespace
{
	constexpr int graph_width = 64;//画像1枚の幅
	constexpr int graph_height = 64;//画像1枚の高さ

	constexpr float p_draw_scale = 3.8f;//プレイヤーの描画スケール
	constexpr int p_max_anim_num = 4;//プレイヤーの最大アニメーション数
	constexpr int p_one_anim_frame = 6;//プレイヤーのアニメーションの待機フレーム数
	constexpr int p_src_y = 5;//プレイヤーの画像縦切り取り位置

	constexpr int e_draw_scale = 4.0f;//敵の描画スケール
	constexpr int e_max_anim_num = 4;//敵の最大アニメーション数
	constexpr int e_one_anim_frame = 6;//敵のアニメーションの待機フレーム数
	constexpr int e_src_y = 5;//敵の画像縦切り取り位置
}

DeathEffect::DeathEffect(int handle, const Vector2& pos, DeathCharactor charactor) :
	m_handle(handle), Effect(pos), m_deathCharactor(charactor)
{
	switch (charactor)
	{
	case DeathCharactor::Player:
		//プレイヤーのアニメーション初期化
		m_playerAnimation.Init(
			m_handle, p_src_y,
			{ graph_width,graph_height },
			p_max_anim_num, p_one_anim_frame,
			p_draw_scale, false);
		break;
	case DeathCharactor::Enemy:
		//敵のアニメーション初期化
		m_enemyAnimation.Init(
			m_handle, e_src_y,
			{ graph_width,graph_height },
			e_max_anim_num, e_one_anim_frame,
			e_draw_scale, false);
		break;
	}
}

void DeathEffect::Update()
{
	switch (m_deathCharactor)
	{
	case DeathCharactor::Player:
		m_playerAnimation.Update();
		if (m_playerAnimation.GetIsEnd())
		{
			m_isDead = true;
		}
		break;
	case DeathCharactor::Enemy:
		m_enemyAnimation.Update();
		if (m_enemyAnimation.GetIsEnd())
		{
			m_isDead = true;
		}
		break;
	}
}

void DeathEffect::Draw(const std::shared_ptr<Camera>& pCamera)
{
	if (m_isDead) return;

	//表示するポジションを計算(カメラオフセット)
	const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
	switch (m_deathCharactor)
	{
	case DeathCharactor::Player:
		//アニメーション描画
		m_playerAnimation.Draw(drawPos, false);
		break;
	case DeathCharactor::Enemy:
		//アニメーション描画
		m_enemyAnimation.Draw(drawPos, false);
		break;
	}
	
}
