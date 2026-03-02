#include "EnemyBullet.h"
#include <DxLib.h>
#include "../General/GameConstants.h"
#include <cmath>
#include "Player.h"
#include "ChargeShotBoss.h"
#include "../Graphics/Camera.h"
#include "../Collider/Circle.h"
#include "../Stages/Stage.h"
#include "../EffectFactory.h"
#include "../SoundManager.h"
#include <cassert>


namespace
{
	constexpr int radius = 50;
	constexpr float speed = 13.0f;
	constexpr int way_num = 5;
	constexpr float angle_30 = DX_PI_F / 6.0f;
	constexpr int bullet_num = 128;
	constexpr int chip_size = 32;//マップチップのサイズ
	constexpr int graph_width = 32;//画像の幅
	constexpr int graph_height = 32;//画像の高さ
	constexpr int src_idx_x = 15;//画像の縦の切り取り位置インデックス
	constexpr int src_idx_y = 6;//画像の縦の切り取り位置インデックス
	constexpr float draw_size = 4.0f;//弾の描画サイズ
	constexpr int max_anim_num = 3;//アニメーションの最大枚数
	constexpr int one_anim_frame = 4;//1コマを見せる時間
}

EnemyBullet::EnemyBullet(std::shared_ptr<EffectFactory> pEffectFactory, std::shared_ptr<Map> pMap):
	Bullet(pEffectFactory,pMap)
{
	m_state = EnemyState::Normal;
	m_circle.SetPos(m_pos);
	m_circle.SetRadius(radius);
	m_handle = LoadGraph("img/game/Bullet/enemy_bullet.png");
	assert(m_handle > - 1);

	m_isRight = false;

	//弾のアニメーション初期化
	m_bulletAnim.Init(
		m_handle,
		src_idx_x, src_idx_y,
		{ graph_width, graph_height },
		max_anim_num, one_anim_frame,
		draw_size, true);
}

EnemyBullet::~EnemyBullet()
{
	//画像の開放
	DeleteGraph(m_handle);
}

void EnemyBullet::Init()
{
	m_dir = { 0.0f,0.0f };
}

void EnemyBullet::Update(GameContext& ctx)
{
	m_animFrame++;
	m_circle.SetRadius(radius);
	//弾が存在している場合のみ更新
	if (m_isAlive)
	{
		//当たり判定を中心に設定する
		m_circle.SetPos(m_pos);

		//弾のアニメーションを更新
		m_bulletAnim.Update();

		//弾の移動処理
		Movement();

		//画面外に出てしまった場合は存在状態を
		//保持している変数にfalseを代入
		const auto& mapSize = ctx.pStage->GetMapSize();
		const auto& boss_startPos = mapSize.w * chip_size - Graphic::screen_width - radius;
		if (m_pos.x < boss_startPos ||
			m_pos.x > mapSize.w * chip_size + radius)
		{
			m_isAlive = false;
			m_bulletAnim.SetFirst();
		}
	}
}

void EnemyBullet::Draw(std::shared_ptr<Camera> pCamera)
{
	if (m_isAlive)
	{
		Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
		m_bulletAnim.Draw(drawPos, m_isRight);

#if _DEBUG
		m_circle.Draw(pCamera);
		DrawFormatString(0, 350, 0xffffff, "EnemyBulletPos X : %f , Y : %f", m_pos.x, m_pos.y);
		DrawFormatString(0, 130, 0xffffff, "ShotDir : %f , %f", m_dir.x, m_dir.y);
		DrawFormatString(0, 95, 0xffffff, "EnemyBulletAlive : %d", m_isAlive);
#endif 
	}
}

void EnemyBullet::Movement()
{
	//弾を移動させる。dirは常に上方向で長さ1なので、
	//正規化はいらない
	Vector2 shotVelocity = m_dir * speed;
	m_pos += shotVelocity;
}

void EnemyBullet::OnDead()
{
	m_isAlive = false;
	m_pEffectFactory->Create(m_pos, EffectType::hitEnemyBullet);
	//消えるときの音を鳴らす
	SoundManager::GetInstance().Play(SoundType::HitShot);
}