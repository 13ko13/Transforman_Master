#define NOMINMAX
#include "ChargeShotBoss.h"
#include "EnemyBullet.h"
#include <DxLib.h>
#include "Player.h"
#include <cassert>
#include <cmath>
#include "../Graphics/Camera.h"
#include "../General/GameConstants.h"
#include "../EffectFactory.h"
#include "../Effect.h"
#include "../Stages/Stage.h"
#include "../Graphics/Animation.h"
#include "../SoundManager.h"

namespace
{
	constexpr int size_width = 144;//幅
	constexpr int size_height = 118;//高さ
	constexpr int graph_width = 48;	//画像1枚の幅
	constexpr int graph_height = 32;//画像1枚の高さ
	constexpr float draw_offset_y = 20.0f;//キャラクターの描画オフセット
	constexpr float p_draw_scale = 5.0f;

	const Vector2 first_pos = { 2380.0f,-32.0f };
	constexpr int attack_cooltime = 120;//攻撃のクールタイム
	constexpr int appear_time = 60;//出現からノーマルに遷移するまでの時間
	constexpr float bullet_pos_offset = 10.0f;
	constexpr float appear_gravity = 6.0f;//出現中の重力
	constexpr float move_speed = 15.0f;//突進の速さ
	constexpr float prev_rush_speed = 0.001f;//突進準備中のほんの少し動く時のスピード
	constexpr int action_cooldown = 120;//次の行動までのフレーム数

	constexpr int prev_rush_frame = 40;//突進準備中の時間
	//アニメーション関連
	constexpr int graph_idx_idle = 0;//待機状態
	constexpr int graph_idx_appear = 0;//出現状態(画像がないので1アイドルと同じ状態)
	constexpr int graph_idx_rush = 0;//突進状態(画像がないので1アイドルと同じ状態)
	constexpr int graph_idx_shot = 1;//弾撃ち状態

	constexpr int anim_wait_frame = 10;//次のアニメーションまでの待機時間
	constexpr int anim_wait_shot = 11;//ショット中の次のアニメーションまでの待機時間
	constexpr int anim_wait_prev_rush = 4;//突進準備中の次のアニメーションまでの待機時間
	constexpr int anim_wait_rush = 4;//突進中のアニメーション待機時間

	constexpr int idle_anim_frame = 4;//待機状態のアニメーション枚数
	constexpr int appear_anim_frame = 4;//出現中のアニメーション枚数
	constexpr int rush_anim_frame = 4;//突進中のアニメーション枚数
	constexpr int shot_anim_frame = 10;//弾撃ちアニメーションの枚数
	constexpr int chip_size = 32;		//マップチップのサイズ

	constexpr int max_hitpoint = 30;//ボスの最大体力

	constexpr int camera_shake_power = 7;//地面に着地したときのカメラの揺れ力

	//エフェクトを出す指定位置
	constexpr float rush_effect_offset_y = 200.0f;

	//チャージショットを受けたときのダメージ
	constexpr int hit_chargeshot_damage = 3;
	constexpr int hit_normalshot_damage = 1;

	constexpr int hit_color_red = 255;//攻撃を受けたときの赤の度合い
	constexpr int hit_color_green = 100;//攻撃を受けたときの緑の度合い
	constexpr int hit_color_blue = 100;//攻撃を受けたときの青の度合い

	//死亡エフェクト
	constexpr int death_effect_pos_y = 80;//死亡エフェクトを出すときの高さ

	//チャージアニメーション
	const Vector2 charge_src = { 64,64 };
	constexpr int max_charge_anim_num = 7;//最大アニメーション枚数
	constexpr int charge_one_anim_num = 5;//アニメーション一枚一枚を表示するフレーム数
	constexpr float charge_draw_size = 5.5f;//表示画像のサイズ

	constexpr int fire_frame_idx = 6;//攻撃アニメーションの発射フレーム

	//ノズルフラッシュアニメーション
	constexpr int flash_src_y = 26;//縦の切り取り位置
	const Vector2 flash_src = { 64,64 };//切り取りサイズ
	constexpr int max_flash_anim_num = 9;//最大アニメーション枚数
	constexpr int flash_one_anim_num = 5;//アニメーション一枚一枚を表示するフレーム数
	constexpr float flash_draw_size = 5.5f;//表示画像のサイズ
	constexpr float flash_draw_pos_offset_x = 50.0f;//描画の際に発射口に合わせるためのオフセット

	//弾の描画オフセット
	constexpr float draw_bullet_offset = 16.0f;

	//ノックバック
	constexpr int knockback_duration = 19;//ノックバックする時間
	constexpr int knockback_speed = 20.0f;//ノックバック時のスピード
	constexpr int knockback_jump = -10.0f;//ノックバック時のy軸へのベクトルの強さ

	//スタン
	constexpr int boss_shake_power = 3;//スタン時の揺らす力
	constexpr int shake_frame = 60;//スタン時に揺らす時間
	constexpr float shake_rate = 1.5f;//揺らすときの速さの割合
	constexpr int shake_range = 5;//揺らす振幅

	//スタンエフェクト
	constexpr float stan_offset_x = size_width / 2;

	//死亡エフェクトを生成する数
	constexpr int top_death_effect_num = 2;//上に出すエフェクトの数
}

ChargeShotBoss::ChargeShotBoss(std::shared_ptr<Map> pMap, std::shared_ptr<EffectFactory> effectfactory,
	StageType& stagaType) :
	EnemyBase(size_width, size_height, pMap, effectfactory),
	m_prevRushTime(0),
	m_actionCooldown(0),
	m_isRushing(false),
	m_isStart(false),
	m_isPlayingFlash(false),
	m_groundNum(0),
	m_drawOffset({0.0f,0.0f}),
	m_shakingTimer(0),
	m_nextEffectTimer(0)
{
	//ボス自身の画像ハンドル
	int handle = LoadGraph("img/game/Enemy/chargeshot_boss.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	//チャージ中の画像ハンドル
	handle = LoadGraph("img/game/Enemy/charge.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	//発射口の光の画像ハンドル
	handle = LoadGraph("img/game/Enemy/nozzle_flash.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	m_pos = first_pos;
	m_state = State::None;
	//体力設定
	m_hitPoint = max_hitpoint;

	m_knockackTimer = 0;
	m_knockbackDir = 0;

	m_stageType = stagaType;
}

ChargeShotBoss::~ChargeShotBoss()
{
	for (auto& handle : m_handles)
	{
		//ハンドルを解放
		DeleteGraph(handle);
	}
}

void ChargeShotBoss::Init()
{
	const Vector2 frameSize = { graph_width ,graph_height };
	//待機状態アニメーションの初期化
	m_idleAnim.Init(m_handles[static_cast<int>(HandleNomber::ChargeShotBoss)], 0, frameSize, idle_anim_frame, anim_wait_frame, p_draw_scale, true);
	//突進準備中アニメーションの初期化
	m_prevRushAnim.Init(m_handles[static_cast<int>(HandleNomber::ChargeShotBoss)], 0, frameSize, rush_anim_frame, anim_wait_prev_rush, p_draw_scale, true);
	//突進中アニメーションの初期化
	m_rushAnim.Init(m_handles[static_cast<int>(HandleNomber::ChargeShotBoss)], 0, frameSize, rush_anim_frame, anim_wait_rush, p_draw_scale, true);
	//弾発射中のアニメーションの初期化
	m_shotAnim.Init(m_handles[static_cast<int>(HandleNomber::ChargeShotBoss)], 1, frameSize, shot_anim_frame, anim_wait_shot, p_draw_scale, false);

	//チャージ中アニメーション初期化
	m_chargeAnim.Init(
		m_handles[static_cast<int>(HandleNomber::Charge)],
		0, { charge_src.x,charge_src.y }, max_charge_anim_num,
		charge_one_anim_num, charge_draw_size, true);

	//ノズルフラッシュアニメーション初期化
	m_flashAnim.Init(
		m_handles[static_cast<int>(HandleNomber::NozzleFlash)],
		flash_src_y, { flash_src.x,flash_src.y }, max_flash_anim_num,
		flash_one_anim_num, flash_draw_size, false);
}

void ChargeShotBoss::Update(GameContext& ctx)
{
	m_animFrame++;
	//地面についている間は地面についた回数をカウント
	if (m_isGround)
	{
		m_groundNum++;
	}
	//一番最初に地面についた瞬間のみカメラを揺らす
	if (m_groundNum == 1)
	{
		ctx.pCamera->OnImpact(camera_shake_power);
	}

	//常にプレイヤーの方向を見る
	const float playerPosX = ctx.pPlayer->GetPos().x;
	//プレイヤーが右にいるなら
	//突進中は突進している方向を向くようにする
	if (!m_isRushing)
	{
		if (m_pos.x < playerPosX)
		{
			//右を見る
			m_isRight = true;
		}
		else//プレイヤーが左にいるなら
		{
			//左を見る
			m_isRight = false;
		}
	}

#ifdef _DEBUG
	//ボタンでステート切り替え
	if (ctx.input.IsTriggered("changeState(enemy)"))
	{
		//m_state = static_cast<State>(static_cast<int>(m_state) + 1 % static_cast<int>(State::MaxState));
		m_hitPoint -= max_hitpoint;
		OnDamage(true);
	}
#endif // DEBUG

	//ステートごとの行動
	switch (m_state)
	{
	case State::None:
		if (!m_isDead)
		{
			Rect chipRect;	//当たったマップチップの矩形
			HitMap(chipRect);//マップとの接地判定
			//押し戻しはしているが地面についていない判定にする(ごり押し)
			m_isGround = false;
		}
		break;
	case State::Appear:
		AppearUpdate();
		break;
	case State::Idle:
		IdleUpdate(ctx);
		break;
	case State::PrevRush:
		PrevRushUpdate(ctx);
		break;
	case State::Rush:
		RushUpdate(ctx);
		break;
	case State::Shot:
		ShotUpdate(ctx.pEnemyBullets, ctx.pPlayer);
		//フラッシュアニメーションを更新
		m_flashAnim.Update();
		break;
	case State::Knockback:
		KnockbackUpdate(ctx);
		break;
	case State::Stan:
		StanUpdate();
		break;
	case State::Death:
		DeathUpdate();
		break;
	}

	if (m_state == State::Appear ||
		m_state == State::Idle)
	{
		//アニメーション更新
		//全枚数のアニメーションを終えたら
		//1枚目の画像に戻す
		m_idleAnim.Update();
	}
	else if (m_state == State::PrevRush)
	{
		//Rush状態より早いアニメーションを行う
		m_prevRushAnim.Update();
	}
	else if (m_state == State::Rush)
	{
		//突進中のみアニメーションを早くする
		m_rushAnim.Update();
	}
	else if (m_state == State::Shot)
	{
		//ショットアニメーション更新
		m_shotAnim.Update();
	}
	else if (m_state == State::Knockback)
	{
		//アイドルアニメーション更新(ノックバック中も)
		m_idleAnim.Update();
	}
	else if (m_state == State::Stan)
	{
		//スタン中もアニメーションはアイドル
		m_idleAnim.Update();
	}

	if (m_isPlayingFlash)
	{
		m_flashAnim.Update();
		if (m_flashAnim.GetIsEnd())
		{
			m_isPlayingFlash = false;
			m_flashAnim.SetFirst();//アニメーション初期化
		}
	}
}

void ChargeShotBoss::Draw(std::shared_ptr<Camera> pCamera)
{
	//生きているなら描画
	if (!m_isDead)
	{
#if _DEBUG
		Charactor::Draw(pCamera);
		DrawFormatString(0, 290, 0xffffff, "ChargeBossPosX:%f,Y:%f", m_pos.x, m_velocity.y);
		DrawFormatString(0, 305, 0xffffff, "ChargeBossPosVelocityX:%f,Y:%f", m_velocity.x, m_velocity.y);
		DrawFormatString(0, 320, 0xffffff, "ChargeBossState:%d", m_state);
		DrawFormatString(0, 370, 0xffffff, "PrevRushTime:%d", m_prevRushTime);
		DrawFormatString(0, 430, 0xffffff, "ChargeBossIsGround:%d", m_isGround);
		DrawFormatString(0, 445, 0xffffff, "ChargeBossHP:%d", m_hitPoint);

#endif

		m_idleAnim.SetOffset({ 0,-draw_offset_y });
		m_rushAnim.SetOffset({ 0,-draw_offset_y });
		m_prevRushAnim.SetOffset({ 0,-draw_offset_y });
		m_shotAnim.SetOffset({ 0,-draw_offset_y });

		//被ダメージ時に赤くする
		if (m_isDamage)
		{
			SetDrawBright(hit_color_red, hit_color_green, hit_color_blue);
		}

		const Vector2 drawPos = m_pos + pCamera->GetDrawOffset() + m_drawOffset;
		//アニメーションの描画
		switch (m_state)
		{
		case State::Appear:
			m_idleAnim.Draw(drawPos, !m_isRight);
			break;
		case State::Idle:
			m_idleAnim.Draw(drawPos, !m_isRight);
			break;
		case State::Rush:
			m_rushAnim.Draw(drawPos, !m_isRight);
			break;
		case State::PrevRush:
			m_prevRushAnim.Draw(drawPos, !m_isRight);
			break;
		case State::Shot:
			m_shotAnim.Draw(drawPos, !m_isRight);
			m_chargeAnim.Draw(drawPos, !m_isRight);
			break;
		case State::Knockback:
			m_idleAnim.Draw(drawPos, !m_isRight);
			break;
		case State::Stan:
			m_idleAnim.Draw(drawPos, !m_isRight);
			break;
		}

		//発射口の光を描画する
		if (m_isPlayingFlash)
		{
			//右向きの場合
			if (m_isRight)
			{
				Vector2 flashDrawPos = m_pos + pCamera->GetDrawOffset();
				flashDrawPos.x += flash_draw_pos_offset_x;
				m_flashAnim.Draw(flashDrawPos, !m_isRight);
			}
			//左向きの場合
			else
			{
				Vector2 flashDrawPos = m_pos + pCamera->GetDrawOffset();
				flashDrawPos.x -= flash_draw_pos_offset_x;
				m_flashAnim.Draw(flashDrawPos, !m_isRight);
			}
		}

		//描画後に明るさを元に戻す
		SetDrawBright(255, 255, 255);
	}
}

void ChargeShotBoss::Attack()
{

}

void ChargeShotBoss::Attack(std::vector<std::shared_ptr<EnemyBullet>>& pBullets,
	std::shared_ptr<Player> pPlayer)
{
	//地面についていないときは行動しない
	if (!m_isGround) return;

	//ステージ1ならチャージショットのみ行う
	//ステージ2なら突進のみおこなう
	//ステージ3なら両方をランダムに行ってくる
	const int stateShot = 0;
	const int stateRush = 1;
	int random = 0;

	switch (m_stageType)
	{
	case StageType::Stage1:
		random = stateShot;
		break;
	case StageType::Stage2:
		random = stateRush;
		break;
	case StageType::Stage3:
		random = GetRand(1);
		break;
	}
	
	
	if (random == stateShot &&
		m_state == State::Idle &&
		m_actionCooldown <= 0)
	{
		m_state = State::Shot;
		m_actionCooldown = action_cooldown;

		//ショットアニメーションを最初から再生する
		m_shotAnim.SetFirst();
		return;
	}
	else if (random == stateRush &&
		m_state == State::Idle &&
		m_actionCooldown <= 0)
	{
		m_state = State::PrevRush;
		m_actionCooldown = action_cooldown;
		m_prevRushTime = prev_rush_frame;
		return;
	}
}

void ChargeShotBoss::OnArrive()
{
	//プレイヤーが到着したときのボスの処理
	//上から出現させてボスっぽくする
	m_pos = first_pos;
	m_appearTime = appear_time;
	m_state = State::Appear;
}

void ChargeShotBoss::OnDamage(bool isChargeShot)
{
	if (isChargeShot)
	{
		if (m_hitPoint < 0) return;//死んでるなら何もしない

		//チャージショットを食らった場合3ダメージ
		m_hitPoint -= hit_chargeshot_damage;
	}
	else
	{
		if (m_hitPoint < 0) return;//死んでるなら何もしない
		//通常ショットなら1ダメージ
		m_hitPoint -= hit_normalshot_damage;
	}
	//HPが0になったらステートを死亡状態にする
	if (m_hitPoint <= 0)
	{
		m_state = State::Death;
		//ボスが死んだのでエフェクトも削除する
		if (auto rushEffect = m_rushEffect.lock())
		{
			rushEffect->Kill();
		}
	}
}

void ChargeShotBoss::OnParried(int dir)
{
	m_knockbackDir = dir;//ノックバックする方向を決める
	m_knockackTimer = knockback_duration;//ノックバックする時間を決める

	//ノックバックするための方向と速度を代入する
	m_velocity.x = m_knockbackDir * knockback_speed;
	m_velocity.y = knockback_jump;

	//揺らす時間をセットする
	m_shakingTimer = shake_frame;

	//ステートをノックバック状態にする
	m_state = State::Knockback;

	//中断されたのでエフェクトを削除する
	if (auto rushEffect = m_rushEffect.lock())
	{
		rushEffect->Kill();
	}
}

void ChargeShotBoss::ShotUpdate(std::vector<std::shared_ptr<EnemyBullet>>& pBullets,
	std::shared_ptr<Player> pPlayer)
{
	//アニメーションの発射フレームでのみ弾を打つ
	const int fireFrameIdx = fire_frame_idx;

	//現在のアニメーションのフレーム数を取得
	int currentFrame = m_shotAnim.GetNowAnimNum();

	//発射フレームに達していなければチャージ中アニメ―ションを更新
	if (currentFrame < fireFrameIdx)
	{
		m_chargeAnim.Update();
		return;
	}
	else
	{
		//弾を撃つ処理
		for (auto& bullet : pBullets)
		{
			//弾を撃つ
			if (!bullet->GetIsAlive())
			{
				//プレイヤーの場所を取得して
				//弾の向きを決定する
				if (pPlayer->GetPos().x > m_pos.x)
				{
					bullet->SetDir({ 1.0f,0.0f });
					//弾の初期位置を設定
					bullet->SetPos({ m_pos.x + size_width / 2, m_pos.y - draw_bullet_offset });
					//向きを設定
					//向きを設定
					if (m_isRight)
					{
						bullet->SetIsRight(false);
					}
					else
					{
						bullet->SetIsRight(true);
					}
				}
				else
				{
					bullet->SetDir({ -1.0f,0.0f });
					//弾の初期位置を設定
					bullet->SetPos({ m_pos.x - size_width / 2, m_pos.y - draw_bullet_offset });

					//向きを設定
					if (m_isRight)
					{
						bullet->SetIsRight(false);
					}
					else
					{
						bullet->SetIsRight(true);
					}
				}
				bullet->OnShot();


				//フラッシュを開始
				m_isPlayingFlash = true;
				m_flashAnim.SetFirst();//アニメーション初期化
				//発射音を再生
				SoundManager::GetInstance().Play(SoundType::BossShot);

				//弾を撃ったので状態をアイドルに戻してbreak
				m_state = State::Idle;
				break;
			}
		}

		//アニメーションが最後まで行ったら
		//状態をアイドルに戻す
		if (m_shotAnim.GetIsEnd())
		{
			m_state = State::Idle;
		}
	}
}

void ChargeShotBoss::RushUpdate(GameContext& ctx)
{
	int stageEnd = ctx.pStage->GetMapSize().w * chip_size;//ステージの終わり
	//生きているなら行動させる
	if (!m_isDead)
	{
		Charactor::Update(ctx);

		Rect chipRect;	//当たったマップチップの矩形
		HitMap(chipRect);//マップとの接地判定

		bool isColWall = m_pos.x <= stageEnd - Graphic::screen_width + chip_size + size_width / 2 ||
			m_pos.x >= stageEnd - chip_size * 2 - size_width / 2;//ボス部屋の端の壁に当たったかどうか
		if (!isColWall && !m_isRushing)
		{
			Vector2 dir = { 0.0f,0.0f };
			//右を向いているなら右方向を持つ
			if (m_isRight)
			{
				dir.x = 1.0f;
			}
			else//左を向いているなら左方向を持つ
			{
				dir.x = -1.0f;
			}
			m_isRushing = true;
			m_velocity += dir.Normalized() * move_speed;
		}

		//ボスが壁がない左端をすり抜けないように押し戻す
		if (m_pos.x < static_cast<float>(stageEnd) - Graphic::screen_width + size_width / 2)
		{
			m_pos.x = static_cast<float>(stageEnd) - Graphic::screen_width + size_width / 2;
			//壁にぶつかった判定で、velを0にする
			m_velocity.x = 0.0f;
		}

		if (isColWall)
		{
			//壁にぶつかったのでエフェクトを消す
			if (auto rushEffect = m_rushEffect.lock())
			{
				rushEffect->Kill();
			}

			//壁にぶつかったエフェクトを出す
			Vector2 effectPos = m_pos;
			effectPos.y -= rush_effect_offset_y;//エフェクトの位置調整
			m_pEffectFactory->Create(effectPos, EffectType::hitWall);

			//ぶつかった時の音を鳴らす
			SoundManager::GetInstance().Play(SoundType::Thunder);

			//ステートをアイドルに戻す
			m_state = State::Idle;
			m_isRushing = false;
			ctx.pCamera->OnImpact(camera_shake_power);
		}
	}
}

void ChargeShotBoss::PrevRushUpdate(GameContext& ctx)
{
	//生きているなら行動させる
	if (!m_isDead)
	{
		m_prevRushTime--;
		//壁にぶつかった瞬間が取れず、条件文的に壁より左にいれば
		//常にカメラが揺れてしまうので、突進準備状態を作って
		//準備中なら、という条件を追加する
		Charactor::Update(ctx);

		Rect chipRect;	//当たったマップチップの矩形
		HitMap(chipRect);//マップとの接地判定

		//二回連続で突進が行われる際に
		//ほんの少しだけ移動させて壁との当たり判定が
		//常に行われないようにする
		Vector2 dir = { 0.0f,0.0f };
		//右を向いているなら右方向を持つ
		if (m_isRight)
		{
			dir.x = 1.0f;
		}
		else//左を向いているなら左方向を持つ
		{
			dir.x = -1.0f;
		}
		m_velocity += dir.Normalized() * prev_rush_speed;

		if (m_prevRushTime < 0)
		{
			m_velocity.x = 0.0f;
			const Vector2 offset = { 0.0f,0.0f };
			m_rushEffect = m_pEffectFactory->CreateFollow(shared_from_this(), EffectType::rush, offset);
			m_state = State::Rush;
			//突進時の音を鳴らす
			SoundManager::GetInstance().Play(SoundType::Rush);
		}
	}
}

void ChargeShotBoss::AppearUpdate()
{
	//ボス部屋到達後に何もいない→上から降ってくる
	//演出をやりたいのでマップチップの当たり判定は行わない
	//降下する時の重力を下げる
	//出現中タイム更新
	m_appearTime--;
	m_velocity.y = appear_gravity;
	m_pos.y += m_velocity.y;
	//降下から数秒経ったら状態をノーマルにする
	if (m_appearTime <= 0)
	{
		m_state = State::Idle;
		m_velocity.y = 0.0f;
		m_isStart = true;
	}
}

void ChargeShotBoss::IdleUpdate(GameContext& ctx)
{
	//生きているなら行動させる
	if (!m_isDead)
	{
		m_actionCooldown--;

		Charactor::Update(ctx);

		Rect chipRect;	//当たったマップチップの矩形
		HitMap(chipRect);//マップとの接地判定

		//ランダムな攻撃方法をとる
		Attack(ctx.pEnemyBullets, ctx.pPlayer);
	}
}

void ChargeShotBoss::KnockbackUpdate(GameContext& ctx)
{
	m_knockackTimer--;

	Charactor::Update(ctx);

	Rect chipRect;	//当たったマップチップの矩形
	HitMap(chipRect);//マップとの接地判定

	// 減速量
	constexpr float decel = 2.0f;

	if (m_knockbackDir > 0) {         // 右へノックバック中（velocity.x > 0 から 0 へ）
		m_velocity.x = std::max(0.0f, m_velocity.x - decel);
	}
	else if (m_knockbackDir < 0) {  // 左へノックバック中（velocity.x < 0 から 0 へ）
		m_velocity.x = std::min(0.0f, m_velocity.x + decel);
	}
	else 
	{
		// dir==0 の安全策
		m_velocity.x = 0.0f;
	}

	if (m_knockackTimer < 0)
	{
		m_state = State::Stan;
		OnStan();
	}
}

void ChargeShotBoss::StanUpdate()
{
	Rect chipRect;	//当たったマップチップの矩形
	HitMap(chipRect);//マップとの接地判定

	m_shakingTimer--;
	if (m_shakingTimer > 0)
	{
		//sin波により揺れる間隔の早さと、振幅を描画オフセットに追加する
		m_drawOffset.x = static_cast<float>(std::sin(static_cast<double>(m_shakingTimer * shake_rate)) * shake_range);
	}
	else
	{
		m_drawOffset = { 0.0f,0.0f };
		//揺れが終了したらアイドルに戻す
		m_state = State::Idle;
		m_velocity = { 0.0f,0.0f };
		m_isRushing = false;
		//ボスの右側にエフェクト
		Vector2 effectPos = m_pos;
		effectPos.x += stan_offset_x;
		m_pEffectFactory->Create(effectPos, EffectType::stanEffect);
		//ボスの左側にエフェクト
		effectPos.x -= stan_offset_x * 2.0f;
		m_pEffectFactory->Create(effectPos, EffectType::stanEffect);
	}
}

void ChargeShotBoss::OnStan()
{
	//ボスの右側にエフェクト
	Vector2 effectPos = m_pos;
	effectPos.x += stan_offset_x;
	m_pEffectFactory->Create(effectPos, EffectType::stanEffect);
	//ボスの左側にエフェクト
	effectPos.x -= stan_offset_x * 2.0f;
	m_pEffectFactory->Create(effectPos, EffectType::stanEffect);

	//スタン時の音を出す
	SoundManager::GetInstance().Play(SoundType::Stan);
}

void ChargeShotBoss::DeathUpdate()
{
	m_nextEffectTimer++;

		//　上に2つ、真ん中に1つ、下に2つエフェクトを出す
		//0フレーム目で左上のエフェクトを生成
		//上の死亡エフェクトを生成
	if (m_nextEffectTimer == 1)
	{
		m_pEffectFactory->Create(
			{ m_pos.x  - size_width / 2, m_pos.y - death_effect_pos_y },
			EffectType::enemyDeath,
			DeathCharactor::Enemy);

		//死亡音を再生
		SoundManager::GetInstance().Play(SoundType::EnemyDeath);
	}
	//20フレーム目を越えたら右上のエフェクトを生成
	else if (m_nextEffectTimer == 10)
	{
		m_pEffectFactory->Create(
			{ m_pos.x + size_width / 2, m_pos.y - death_effect_pos_y },
			EffectType::enemyDeath,
			DeathCharactor::Enemy);

		//死亡音を再生
		SoundManager::GetInstance().Play(SoundType::EnemyDeath);
	}
	//40フレーム目を越えたら真ん中のエフェクトを生成
	else if (m_nextEffectTimer == 20)
	{
		m_pEffectFactory->Create(
			{ m_pos.x , m_pos.y  },
			EffectType::enemyDeath,
			DeathCharactor::Enemy);

		//死亡音を再生
		SoundManager::GetInstance().Play(SoundType::EnemyDeath);
	}
	//60フレーム目を越えたら左下のエフェクトを生成
	else if (m_nextEffectTimer == 30)
	{
		m_pEffectFactory->Create(
			{ m_pos.x - size_width / 2, m_pos.y + death_effect_pos_y },
			EffectType::enemyDeath,
			DeathCharactor::Enemy);

		//死亡音を再生
		SoundManager::GetInstance().Play(SoundType::EnemyDeath);
	}
	else if (m_nextEffectTimer == 40)
	{
		m_pEffectFactory->Create(
			{ m_pos.x + size_width / 2, m_pos.y + death_effect_pos_y },
			EffectType::enemyDeath,
			DeathCharactor::Enemy);

		//死亡音を再生
		SoundManager::GetInstance().Play(SoundType::EnemyDeath);

		//HPが0になっているなら死亡フラグを立てる
		if (m_hitPoint <= 0)
		{
			m_isDead = true;
		}
	}
}
