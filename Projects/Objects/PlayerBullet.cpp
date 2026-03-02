#include "PlayerBullet.h"
#include <DxLib.h>
#include "../General/GameConstants.h"
#include "../Graphics/Camera.h"
#include "../Collider/Circle.h"
#include "EnemyBase.h"
#include "../Stages/Stage.h"
#include "../EffectFactory.h"
#include "../Map.h"
#include "../SoundManager.h"
#include <cassert>

namespace
{
	constexpr float speed = 10.0f;
	constexpr int normal_shot_radius = 15;
	constexpr int charge_shot_radius = 25;

	//アニメーション
	//Normal
	constexpr int normal_anim_idx_y = 0;//アニメーション切り取りY位置
	constexpr int normal_max_anim_num = 4;//アニメーション枚数
	constexpr int normal_one_anim_frame = 5;//1アニメーションあたりのフレーム数
	constexpr int normal_graph_width = 16;//画像1枚の幅
	constexpr int normal_graph_height = 16;//画像1枚の高さ
	constexpr float normal_draw_scale = 3.0f;//描画スケール

	//Charge
	constexpr int charge_anim_idx_y = 6;//アニメーション切り取りY位置
	constexpr int charge_anim_idx_x = 15;//アニメーション切り取りX位置
	constexpr int charge_max_anim_num = 3;//アニメーション枚数
	constexpr int charge_one_anim_frame = 4;//1アニメーションあたりのフレーム数
	constexpr int charge_graph_width = 32;//画像1枚の幅
	constexpr int charge_graph_height = 32;//画像1枚の高さ
	constexpr float charge_draw_scale = 3.0f;//描画スケール

	//Fire
	constexpr int fire_width = 150;
	constexpr int fire_height = 30;
	constexpr float flame_life_time = 30.0f;//炎の残る時間

	constexpr int chip_size = 32;//マップチップのサイズ
}

PlayerBullet::PlayerBullet(std::shared_ptr<EffectFactory> pEffectFactory, std::shared_ptr<Map> pMap) :
	Bullet(pEffectFactory, pMap),
	m_animFrame(0),
	m_isRight(false),
	m_flameLifeTime(0.0f)
{
	m_bulletType = BulletType::Normal;
	m_circle.SetPos(m_pos);
	m_circle.SetRadius(normal_shot_radius);
	//左上を基準に当たり判定を設定
	m_rect.SetLT(
		m_pos.x, m_pos.y,
		fire_width, fire_height);

	//画像をロード
	int handle = -1;

	handle = LoadGraph("img/game/bullet/player_bullet.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	handle = LoadGraph("img/game/bullet/player_charge_bullet.png");
	assert(handle > -1);
	m_handles.push_back(handle);
}

PlayerBullet::~PlayerBullet()
{
	//画像の開放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
}

void PlayerBullet::Init()
{
	//通常弾アニメーション初期化
	m_normalAnim.Init(
		m_handles[static_cast<int>(HandleNumber::Normal)],
		0, { normal_graph_width ,normal_graph_height },
		normal_max_anim_num, normal_one_anim_frame,
		normal_draw_scale, true);
	//チャージ弾アニメーション初期化
	m_chargeAnim.Init(
		m_handles[static_cast<int>(HandleNumber::Charge)],
		charge_anim_idx_x, charge_anim_idx_y,
		{ charge_graph_width, charge_graph_height },
		charge_max_anim_num, charge_one_anim_frame,
		charge_draw_scale, true);
}

void PlayerBullet::Update(GameContext& ctx)
{
	switch (m_bulletType)
	{
	case BulletType::Normal:
		m_circle.SetRadius(normal_shot_radius);

		if (m_isAlive)
		{
			if (m_isRight)
			{
				m_dir = { 1.0f, 0.0f };
			}
			else
			{
				m_dir = { -1.0f, 0.0f };
			}
			m_circle.SetPos(m_pos);

			//弾を移動させる。dirは
			//常に上方向で長さ1なので、
			//正規化はいらない
			Vector2 shotVelocity = m_dir * speed;
			m_pos = m_pos + shotVelocity;

			//ステージ外に出てしまった場合は存在状態を
			//保持している変数にfalseを代入
			const auto& mapSize = ctx.pStage->GetMapSize();
			if (m_pos.x < 0 - normal_shot_radius ||
				m_pos.x > mapSize.w * chip_size + normal_shot_radius)
			{
				m_isAlive = false;
			}
			//アニメーションの更新
			m_normalAnim.Update();
		}
		break;
	case BulletType::Charge:
		m_circle.SetRadius(charge_shot_radius);

		if (m_isAlive)
		{
			if (m_isRight)
			{
				m_dir = { 1.0f, 0.0f };
			}
			else
			{
				m_dir = { -1.0f, 0.0f };
			}

			m_circle.SetPos(m_pos);

			//弾を移動させる。dirは常に上方向で長さ1なので、
			//正規化はいらない
			Vector2 shotVelocity = m_dir * speed;
			m_pos = m_pos + shotVelocity;

			//画面外に出てしまった場合は存在状態を
			//保持している変数にfalseを代入
			//ステージ外に出てしまった場合は存在状態を
			//保持している変数にfalseを代入
			const auto& mapSize = ctx.pStage->GetMapSize();
			if (m_pos.x < 0 - normal_shot_radius ||
				m_pos.x > mapSize.w * chip_size + normal_shot_radius)
			{
				m_isAlive = false;
			}
			//アニメーションの更新
			m_chargeAnim.Update();
		}
		break;
	case BulletType::Fire:
		m_flameLifeTime--;

		//左上を基準に当たり判定を設定
		//右を向いているときは自分の右側
		//左を向いているときは自分の左側に
		//火炎放射が出るようにする
		if (m_isRight)
		{
			m_rect.SetLT(
				m_pos.x,
				m_pos.y,
				fire_width, fire_height);
		}
		else
		{
			m_rect.SetLT(
				m_pos.x,
				m_pos.y,
				-fire_width, fire_height);
		}

		//炎の残り火時間が0以下になったら消えるようにする
		if (m_flameLifeTime <= 0)
		{
			m_isAlive = false;
		}
		break;
	}
}

void PlayerBullet::Draw(std::shared_ptr<Camera> pCamera)
{
	if (m_isAlive)
	{
		switch (m_bulletType)
		{
		case BulletType::Normal:
#if _DEBUG
			//当たり判定を描画する
			m_circle.Draw(pCamera);

			DrawFormatString(
				0, 45, 0xffffff,
				"PlayerBulletPos X:%f , Y:%f",
				m_pos.x, m_pos.y);
#endif
			//アニメーションのdraw
			m_normalAnim.Draw(
				{ m_pos.x + pCamera->GetDrawOffset().x,
				m_pos.y + pCamera->GetDrawOffset().y }, m_isRight);
			break;

		case BulletType::Charge:
#if _DEBUG
			//当たり判定を描画する
			m_circle.Draw(pCamera);
#endif
			//アニメーションの描画
			m_chargeAnim.Draw(
				{ m_pos.x + pCamera->GetDrawOffset().x,
				m_pos.y + pCamera->GetDrawOffset().y }, !m_isRight
			);
			break;

		case BulletType::Fire:
#if _DEBUG
			//当たり判定を描画する
			m_rect.Draw(0xffffff, false, pCamera);
			DrawFormatString(0, 270, 0xffffff, "FlameLifeTime : %f", m_flameLifeTime);
#endif
			break;
		}
	}
}

void PlayerBullet::OnFlame()
{
	//炎が消えるまでの時間をセットする
	m_flameLifeTime = flame_life_time;
}

void PlayerBullet::OnDead()
{
	m_isAlive = false;
	m_pEffectFactory->Create(m_pos, EffectType::hitPlayerBullet);

	//音を再生
	SoundManager::GetInstance().Play(SoundType::HitShot);
}