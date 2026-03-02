#define NOMINMAX
#include "Player.h"
#include "../Utility/Geometry.h"
#include "DxLib.h"
#include "../General/GameConstants.h"
#include "PlayerBullet.h"
#include "../Graphics/Camera.h"
#include "../EffectFactory.h"
#include "../SoundManager.h"
#include <cassert>
#include "../../Dxlib_h/EffekseerForDXLib.h"

namespace
{
	constexpr float ground = Graphic::screen_height - 220;	//地面の高さ(仮)
	constexpr int move_speed = 5;							//移動速度
	constexpr int debug_speed = 10;							//デバッグ用でプレイヤーのスピードを変えたときの値
	constexpr int size_width = 40;							//キャラクターの横幅
	constexpr int size_height = 50;							//キャラクターの高さ
	constexpr int graph_width = 40;							//画像の横切り取りサイズ
	constexpr int graph_height = 40;						//画像の縦切り取りサイズ
	constexpr int rect_offset_y = 12;						//キャラクターの５場所と矩形の場所を合わせる(微妙に頭の上の当たり判定が大きくなってしまうため)
	constexpr double p_draw_scale = 2.0f;						//描画スケール	

	constexpr int knockback_duration = 15;					//ノックバックする時間
	constexpr float knockback_speed = 15.0f;				//ノックバックするときのスピード
	constexpr float knockback_jump = -7.0f;					//縦のノックバック力

	constexpr int max_jump_frame = 15;						//最大ジャンプ入力時間
	constexpr float jump_power = 12.0f;						//ジャンプ力

	constexpr int shot_cooltime = 13;						//ショットのクールタイム
	constexpr int judg_charge_frame = 15;					//チャージ中と判定するまでの入力フレーム数
	constexpr int prev_charge_time = 80;					//ショットからチャージショットになるまでの猶予フレーム
	constexpr int max_blink_time = 60;						//点滅するフレーム数
	constexpr int flame_motion_frame = 30;					//火炎放射中の時間

	constexpr int parry_i_frame = 60;						//パリィ後の無敵時間
	constexpr int parry_cooltime = 360;						//パリィのクールタイム

	//アニメーション用のグラフのインデックス
	constexpr int graph_index_idle = 0;
	constexpr int graph_index_walk = 1;
	constexpr int graph_index_shot = 2;
	constexpr int graph_index_damage = 3;
	constexpr int graph_index_deth = 7;
	constexpr int graph_index_jump = 8;

	//アニメーション関係
	constexpr int anim_wait_frame = 7;						//次のアニメーションまでの待機時間
	constexpr int idle_anim_frame = 4;						//アイドルアニメーションの枚数
	constexpr int flame_anim_frame = 1;						//火炎放射中のアニメーションの枚数
	constexpr int walk_anim_frame = 7;						//歩き状態のアニメーションの枚数
	constexpr int damage_anim_frame = 8;					//食らい状態のアニメーション枚数
	constexpr int deth_anim_frame = 21;						//死んだときのアニメーション枚数
	constexpr int jump_anim_frame = 3;						//ジャンプの時のアニメーション枚数

	//HP
	constexpr int max_hit_point = 5;						//HP

	//スポーン地点
	const Vector2 spawn_pos = { 180.0f,583.0f };

	//チャージアニメーション
	const Vector2 charge_src = { 64,64 };
	constexpr int max_charge_anim_num = 7;//最大アニメーション枚数
	constexpr int charge_one_anim_num = 5;//アニメーション一枚一枚を表示するフレーム数
	constexpr float charge_draw_size = 2.3f;//表示画像のサイズ

	//チャージ完了アニメーション
	const Vector2 charged_src = { 64,64 };//切り抜き位置
	constexpr int max_charged_anim_num = 7;//最大アニメーション枚数
	constexpr int charged_one_anim_num = 5;//アニメーション一枚一枚を表示するフレーム数
	constexpr float charged_draw_size = 2.0f;//表示画像サイズ
	constexpr int charged_srcY = 7;		//縦切り取り位置

	constexpr int blinking_timer = 5;//無敵中の点滅する時間

	constexpr float barrior_draw_offset_y = 27.0f;//エフェクシアのバリアエフェクトを描画する際のオフセット

	const Vector2 effect_pos_offset = { 40.0f,0.0f };

	//チャージし始めてからチャージ音を鳴らすまでの時間
	constexpr int charge_se_start = 30;

	//プレイヤーがピンチになるときのHP
	constexpr int pinch_hp = 3;

	//パリィ時のバリア
	constexpr float barrior_width = 170.0f;
	constexpr float barrior_height = 80.0f;
	constexpr float barrior_pos_offset_y = 20.0f;
}

Player::Player(std::shared_ptr<Map> pMap, std::shared_ptr<EffectFactory> effectfactory) :
	Charactor(size_width, size_height, pMap, effectfactory),
	m_isJumping(false),
	m_isPrevJump(false),
	m_isCharging(false),
	m_isCharged(false),
	m_isArrive(false),
	m_isInvincible(false),
	m_isCanAction(true),
	m_isStartDash(false),
	m_isChargeSound(false),
	m_isJumpSound(false),
	m_jumpFrame(0),
	m_shotCooltime(0),
	m_flameThrowerCT(0),
	m_flameThrowCount(0),
	m_knockackTimer(0),
	m_blinkingTimer(0),
	m_state(PlayerState::Idle),
	m_weaponType(WeaponType::Normal),
	m_prevChargeFrame(0),
	m_animSrcX(0),
	m_animSrcY(0),
	m_animIdx(0),
	m_damageAnimFrame(0),
	m_knockbackDir(0),
	m_parryCooltime(0),
	m_iFrameTimer(0),
	m_cameraOffset({ 0,0 }),
	m_playingEffectHandle(-1)
{
	int handle = LoadGraph("img/game/Player/transforman_player.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	handle = LoadGraph("img/game/Player/Charge.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	handle = LoadGraph("img/game/Player/Charged.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	m_hitPoint = max_hit_point;//HPを設定した
	m_isRight = true;//右を向かせておく

	StopEffekseer2DEffect(m_playingEffectHandle);
	m_playingEffectHandle = -1;

	// エフェクトリソースを読み込む
	// ループで無限再生されるエフェクトは、パーティクル自体の生成数が無限だったり、
	// パーティクルの寿命が無限だったりする
	m_effectResourceHandle = LoadEffekseerEffect("Effekseer_Effect/barrior.efk");
	assert(m_effectResourceHandle > -1);

	//チャージ中アニメーション初期化
	m_chargeAnim.Init(
		m_handles[static_cast<int>(HandleNomber::ChargeHandle)],
		0, { charge_src.x,charge_src.y }, max_charge_anim_num,
		charge_one_anim_num, charge_draw_size, true);

	//チャージ完了アニメーション初期化
	m_chargedAnim.Init(
		m_handles[static_cast<int>(HandleNomber::ChargedHandle)],
		charged_srcY, { charged_src.x,charged_src.y }, max_charged_anim_num,
		charged_one_anim_num, charged_draw_size, true);

	//バリアの矩形を当たらないところに初期化する
	m_shieldRect.SetCenter(0, 0, 0, 0);
}

Player::~Player()
{
	DeleteGraph(m_handle);

	StopEffekseer2DEffect(m_playingEffectHandle);
	m_playingEffectHandle = -1;
}

void Player::Init()
{
	m_pos = spawn_pos;
}

void Player::Update(GameContext& ctx)
{
#if _DEBUG
	//デバッグ処理
	Debug(ctx.input);
#endif

	if (m_hitPoint <= 0)
	{
		m_state = PlayerState::Dead;//死亡状態にする
	}

	//フレームを更新
	m_frame++;
	//アニメーションフレームの更新
	m_animFrame++;

	//クールタイムを更新して0以下になったら
	//ショット可能状態にする
	m_shotCooltime--;
	//クールタイムを更新して0以下になったら
	//パリィ可能状態にする
	m_parryCooltime--;


	if (m_prevChargeFrame > prev_charge_time)
	{
		m_isCharged = true;
	}

	//クールタイムが0以下の場合ショットの準備を進める
	if (m_shotCooltime <= 0)
	{
		PrevShot(ctx.input, ctx.pPlayerBullets);
	}

	//前のフレームでチャージが完了したかを取得
	bool wasChargedPrev = m_isCharged;

	if (m_prevChargeFrame > prev_charge_time)
	{
		m_isCharged = true;
	}
	else
	{
		m_isCharged = false;
	}

	//前のフレームでも完了していて、今回のフレームでもチャージ完了していたら
	if (!wasChargedPrev && m_isCharged)
	{
		//アニメーションを初期に戻す
		m_chargeAnim.SetFirst();
		SoundManager::GetInstance().Play(SoundType::PlayerChargeFinished);
		SoundManager::GetInstance().StopSound(SoundType::PlayerCharge);
	}

	if (m_isCharged)
	{
		//チャージ完了時のみチャージ完了エフェクトの更新を行う
		m_chargedAnim.Update();
		//チャージ完了エフェクトのアニメーションが終了したら
		//アニメーションを最初に戻す
		if (m_chargedAnim.GetIsEnd())
		{
			m_isCharged = false;
		}
	}

	//ダメージ状態中または火炎放射中は行動できないようにする
	if (m_isCanAction)
	{
		//移動
		Move(ctx.input);
		//ジャンプを長押ししていて、地面についているなら
		//ジャンプ中
		if (ctx.input.IsPressed("jump"))
		{
			if (m_isGround)
			{
				m_isJumping = true;
				m_isJumpSound = false;
			}
			//ジャンプ
			Jump();
		}
		else
		{
			//ジャンプが入力されていないなら
			//ジャンプ中を解除
			m_jumpFrame = 0;
			m_isJumping = false;
		}

		//プレイヤーがパリィボタンを押したらパリィを発動
		if (ctx.input.IsTriggered("parry") &&
			m_parryCooltime < 0 &&
			m_isGround &&
			m_state != PlayerState::Damage)
		{
			//状態をパリィに変更
			m_state = PlayerState::Parry;
			OnParry();
		}
	}

	Charactor::Update(ctx);
	Rect chipRect;	//当たったマップチップの矩形
	HitMap(chipRect);//マップとの接地判定
	//火炎放射もクールタイムを更新して0以下になったら
	//放射可能状態にする
	m_flameThrowerCT--;

	//無敵中
	if (m_isInvincible)
	{
		m_blinkingTimer--;
		if (m_blinkingTimer < 0)
		{
			m_isInvincible = false;
		}
	}

	//None,
	//Idle,
	//Walk,
	//Shot,
	//Jump,
	//ChageShot,
	//Climb,
	//Fire
	//アニメーションのフレーム数から表示したいコマ番号を計算で求める
	//anim_wait_frame進むごとにアニメーションが1枚進む
	int AnimNo = m_animFrame / anim_wait_frame;
	m_animSrcX = graph_width * AnimNo;//現在表示したいStateのアニメーションの横切り取り位置
	m_animSrcY = graph_height * m_animIdx;//現在表示したいStateのアニメーションの縦切り取り位置
	int animMax = 0;
	switch (m_state)
	{
	case PlayerState::Idle:
		m_animSrcY = graph_height * graph_index_idle;
		animMax = idle_anim_frame;
		break;
	case PlayerState::Walk:
		m_animSrcY = graph_height * graph_index_walk;
		animMax = walk_anim_frame;
		break;
	case PlayerState::Shot:
	case PlayerState::ChargeShot:

		break;
	case PlayerState::Jump:
		m_animSrcY = graph_height * graph_index_jump;
		animMax = jump_anim_frame;
		break;
	case PlayerState::Parry:
	{
		//バリアの矩形を更新する
		m_shieldRect.SetCenter(
			m_pos.x, m_pos.y - barrior_pos_offset_y,
			barrior_width, barrior_height);

		//パリィ更新処理
		m_iFrameTimer--;
		if (m_iFrameTimer < 0)
		{
			//ステートを戻す
			m_state = PlayerState::Idle;

			if (m_playingEffectHandle >= 0)
			{
				//エフェクシアのエフェクトを停止する
				auto manager = GetEffekseer2DManager();
				manager.Get()->SendTrigger(m_playingEffectHandle, 1);
				m_playingEffectHandle = -1;

				//バリアの矩形を無効化する
				m_shieldRect.SetCenter(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}
	break;
	case PlayerState::Fire:
		//行動不能にする
		m_isCanAction = false;
		m_animSrcY = graph_height * graph_index_shot;
		animMax = flame_anim_frame;
		m_velocity.x = 0.0f;

		m_flameThrowCount--;
		if (m_flameThrowCount <= 0)
		{
			m_state = PlayerState::Idle;
			m_isCanAction = true;
		}
		break;
	case PlayerState::Damage:
		//無敵を有効にする
		m_isInvincible = true;
		//ノックバックさせる
		UpdateKnockback();
		//行動不能にする
		m_isCanAction = false;
		//画像の縦切り取り位置
		m_animSrcY = graph_height * graph_index_damage;
		//アニメーションの一番最後のフレーム数を保存
		animMax = damage_anim_frame;
		//ダメージのみ切り取り位置を補正
		m_animSrcX = graph_width * ((int)m_damageAnimFrame / anim_wait_frame);
		//ダメージアニメーションが終わったら
		//StateをIdleに切り替える
		m_damageAnimFrame++;
		if (m_damageAnimFrame >= animMax * anim_wait_frame)
		{
			//idleに戻った後の無敵時間を設定
			m_blinkingTimer = max_blink_time;
			m_damageAnimFrame = 0;
			m_isCanAction = true;
			m_state = PlayerState::Idle;
		}
		break;
	case PlayerState::Dead:
		UpdateKnockback();
		//行動不能にする
		m_isCanAction = false;
		m_isDead = true;
		m_animSrcY = graph_height * graph_index_deth;
		animMax = deth_anim_frame;
		//アニメーションが最後まで行ったら
		if (m_animFrame >= animMax * anim_wait_frame)
		{
			m_animFrame = animMax * anim_wait_frame - 1;
		}
		break;
	}
	//現在のアニメーションのフレーム数が
	//現在のステートの描画枚数を超えたら
	//現在のアニメーションのフレーム数を0にする
	if (m_animFrame >= animMax * anim_wait_frame)
	{
		m_animFrame = 0;
	}


	//ボス部屋に到着している場合
	//そこから出られないように
	//プレイヤーを押し戻しする
	if (m_isArrive &&
		m_pos.x < Graphic::screen_width + size_width * 0.5f)
	{
		m_pos.x = Graphic::screen_width + size_width * 0.5f;
	}

	m_cameraOffset = ctx.pCamera->GetDrawOffset();
	const Vector2 drawPos = m_cameraOffset + m_pos;

	if (m_playingEffectHandle >= 0) // 再生中エフェクトのハンドルがあれば.
	{
		// 再生中のエフェクトを移動
		SetPosPlayingEffekseer2DEffect(m_playingEffectHandle, drawPos.x, drawPos.y + barrior_draw_offset_y, 0);
	}

	m_parryCooltime = std::max(0, m_parryCooltime - 1);
}

void Player::Draw(std::shared_ptr<Camera> pCamera)
{
#if _DEBUG
	Charactor::Draw(pCamera);
	DrawFormatString(0, 0, 0xffffff, "Frame:%d", m_frame);
	//DrawFormatString(0, 15, 0xffffff, "PlayerPosX:%f, Y: %f", m_pos.x, m_pos.y);
	//DrawFormatString(0, 30, 0xffffff, "IsRight:%d", m_isRight);
	//DrawFormatString(0, 45, 0xffffff, "JumpFrame : %d", m_jumpFrame);
	//DrawFormatString(0, 60, 0xffffff, "ShotCoolTime:%d", m_shotCooltime);
	//DrawFormatString(0, 150, 0xffffff, "PrevChargeFrame:%d", m_prevChargeFrame);
	//DrawFormatString(0, 165, 0xffffff, "Ground : %d", m_isGround);
	//DrawFormatString(0, 195, 0xffffff, "Velocity(%f , %f)", m_velocity.x, m_velocity.y);
	DrawFormatString(0, 230, 0xffffff, "PlayerState : %d", m_state);
	//DrawFormatString(0, 225, 0xffffff, "DamageAnimFrame : %f", m_damageAnimFrame);
	//DrawFormatString(0, 240, 0xffffff, "BlinkingTimer : %d", m_blinkingTimer);
	//DrawFormatString(0, 255, 0xffffff, "WeaponType : %d", m_weaponType);
	//DrawFormatString(0, 385, 0xffffff, "IsCanAction : %d", m_isCanAction);
	//DrawFormatString(0, 400, 0xffffff, "ParryTimer : %d", m_iFrameTimer);
	//DrawFormatString(0, 415, 0xffffff, "ParryCooltime : %d", m_parryCooltime);
	DrawFormatString(0, 15, 0xffffff, "HP: %d", m_hitPoint);
#endif

	//操作方法
	/*DrawFormatString(Graphic::screen_width - 160, 64, 0xffffff, "Jump : B");
	DrawFormatString(Graphic::screen_width - 160, 64 + 15, 0xffffff, "Shot : A");
	DrawFormatString(Graphic::screen_width - 160, 64 + 30, 0xffffff, "Move : Lスティック");
	DrawFormatString(Graphic::screen_width - 160, 64 + 45, 0xffffff, "Parry : X");*/

	//バリアの矩形を可視化
#ifdef _DEBUG
	m_shieldRect.Draw(0xffaaff, false, pCamera);
#endif // DEBUG


	//チャージ中のみチャージアニメーションを描画する
	if (m_isCharging)
	{
		//チャージ完了時は表示しない
		if (!m_isCharged)
		{
			const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
			m_chargeAnim.Draw(drawPos, m_isRight);
		}
		else
		{
			const Vector2 drawPos = m_pos + pCamera->GetDrawOffset();
			m_chargedAnim.Draw(drawPos, m_isRight);
		}
	}

	//ダメージ受けたときは一定時間表示する→しないを繰り返して
	//無敵時間を表す
	if (m_isInvincible)
	{
		//点滅
		if (m_blinkingTimer % blinking_timer == 0)
		{
			DrawRectRotaGraph(
				static_cast<int>(m_pos.x + pCamera->GetDrawOffset().x),
				static_cast<int>(m_pos.y + pCamera->GetDrawOffset().y) - rect_offset_y, //表示位置
				m_animSrcX, m_animSrcY,													//切り取り開始位置
				graph_width, graph_height,												//切り取りサイズ
				p_draw_scale, 0.0,														//拡大率、回転角度
				m_handles[static_cast<int>(HandleNomber::PlayerHandle)],				//画像ハンドル
				true,																	//透明度
				!m_isRight																//反転
			);
		}
		else
		{

		}
	}
	else
	{
		//無敵中じゃないときは常に表示
		DrawRectRotaGraph(
			static_cast<int>(m_pos.x + pCamera->GetDrawOffset().x),
			static_cast<int>(m_pos.y + pCamera->GetDrawOffset().y) - rect_offset_y, //表示位置
			m_animSrcX, m_animSrcY,													//切り取り開始位置
			graph_width, graph_height,												//切り取りサイズ
			p_draw_scale, 0.0,														//拡大率、回転角度
			m_handles[static_cast<int>(HandleNomber::PlayerHandle)],				//画像ハンドル
			true,																	//透明度
			!m_isRight																//反転
		);
	}

}

void Player::OnArriveEnemy()
{
	m_isArrive = true;
	m_isCanAction = false;
	m_isRight = true;
	m_velocity.x = 0.0f;
	m_state = PlayerState::Idle;

	//エフェクシアのエフェクトを停止する
	auto manager = GetEffekseer2DManager();
	manager.Get()->SendTrigger(m_playingEffectHandle, 1);
}

void Player::Jump()
{
	if (m_state == PlayerState::Damage ||
		m_state == PlayerState::Parry ||
		m_state == PlayerState::Dead) return;
	//ジャンプを長押ししていないなら飛ばす
	if (!m_isJumping) return;

	m_jumpFrame++;
	m_isGround = false;
	if (m_jumpFrame < max_jump_frame)
	{
		m_velocity.y = -jump_power;
		//ジャンプ音を鳴らしていないなら鳴らす
		if (!m_isJumpSound)
		{
			//ジャンプ音を再生
			SoundManager::GetInstance().Play(SoundType::Jump);
			m_isJumpSound = true;
		}
	}
}

void Player::Move(Input& input)
{
	//ダメージを受けているときと炎を放っているときと
	//パリィを行っているときは
	//処理を行わない
	if (m_state == PlayerState::Damage ||
		m_state == PlayerState::Fire ||
		m_state == PlayerState::Parry ||
		m_state == PlayerState::Dead) return;

	Vector2 dir = { 0.0f,0.0f };//プレイヤーの速度ベクトル
	m_velocity.x = 0.0f;
	if (input.IsPressed("right"))
	{
		m_state = PlayerState::Walk;
		dir.x = 1.0f;
		m_isRight = true;
		//ダッシュエフェクトを生成
		if (m_isGround && !m_isStartDash)
		{
			m_isStartDash = true;
			m_pEffectFactory->Create(
				{ m_pos.x, m_pos.y + 12.0f },
				EffectType::dash,
				!m_isRight
			);
		}
	}
	else if (input.IsPressed("left"))
	{
		m_state = PlayerState::Walk;
		dir.x = -1.0f;
		m_isRight = false;

		//ダッシュエフェクトを生成
		if (m_isGround && !m_isStartDash)
		{
			m_isStartDash = true;
			m_pEffectFactory->Create(
				{ m_pos.x, m_pos.y + 12.0f },
				EffectType::dash,
				!m_isRight
			);
		}
	}
	else
	{
		m_state = PlayerState::Idle;
		m_isStartDash = false;
	}
	int speed = move_speed;
#ifdef _DEBUG
	//デバッグ用のプレイヤーのスピードアップ
	if (input.IsPressed("playerSpeedUp"))
	{
		speed = debug_speed;
	}
#endif // _DEBUG
	//ディレクションを正規化してプレイヤーのスピードをかけて
	//ポジションに足してあげる移動処理
	m_velocity += dir.Normalized() * static_cast<float>(speed);
}

void Player::Shot(std::vector<std::shared_ptr<PlayerBullet>>& pBullets)
{
	for (auto& bullet : pBullets)
	{
		if (!bullet->GetIsAlive())
		{
			//弾が存在していない場合、弾を発射する
			if (m_isRight)
			{
				//右向き
				bullet->SetPos({ m_pos.x + size_width / 2, m_pos.y });
			}
			else
			{
				//左向き
				bullet->SetPos({ m_pos.x - size_width / 2 , m_pos.y });
			}
			bullet->SetType(BulletType::Normal);
			bullet->OnShot();
			bullet->SetIsRight(m_isRight);
			m_chargeAnim.SetFirst();
			SoundManager::GetInstance().Play(SoundType::NormalShot);
			break;	//1発撃ったらループを抜ける
		}
	}

}

void Player::ChargeShot(std::vector<std::shared_ptr<PlayerBullet>>& pBullets)
{
	for (auto& bullet : pBullets)
	{
		if (!bullet->GetIsAlive())
		{
			//弾が存在していない場合、弾を発射する
			if (m_isRight)
			{
				//右向き
				bullet->SetPos({ m_pos.x + size_width / 2 , m_pos.y });
			}
			else
			{
				//左向き
				bullet->SetPos({ m_pos.x - size_width / 2 , m_pos.y });
			}
			bullet->SetType(BulletType::Charge);
			bullet->OnShot();
			bullet->SetIsRight(m_isRight);
			m_chargeAnim.SetFirst();
			//チャージショット放出音を再生
			SoundManager::GetInstance().Play(SoundType::PlayerChargeShot);
			break;	//1発撃ったらループを抜ける
		}
	}
}

void Player::FireShot(std::vector<std::shared_ptr<PlayerBullet>>& pBullets)
{
	//地面についているときのみ発射OKとする
	if (!m_isGround) return;

	for (auto& bullet : pBullets)
	{
		if (bullet->GetIsAlive()) return;

		bullet->OnFlame();
		//弾が存在していない場合、弾を発射する
		if (m_isRight)
		{
			//右向き
			bullet->SetPos({ m_pos.x + size_width / 2, m_pos.y });
			////状態遷移
			//m_state = PlayerState::ChargeShot;
		}
		else
		{
			//左向き
			bullet->SetPos({ m_pos.x - size_width / 2 , m_pos.y });
			//状態遷移
			//m_state = PlayerState::ChargeShot;
		}
		bullet->SetType(BulletType::Fire);
		bullet->OnShot();
		bullet->SetIsRight(m_isRight);
		m_chargeAnim.SetFirst();
		//火炎放射中の時間を計測する変数に代入
		m_flameThrowCount = flame_motion_frame;
		break;	//1発撃ったらループを抜ける

	}
}

void Player::PrevShot(Input& input, std::vector<std::shared_ptr<PlayerBullet>>& pBullets)
{
	if (m_state != PlayerState::Damage &&
		m_state != PlayerState::Dead)
	{
		//ボタンが一定フレーム以上
		//長押しされたらチャージショットの判定にする
		//チャージショットの猶予時間を超えたらチャージショット
		//それ以外は通常ショット
		//もしくは火炎放射
		if (input.IsPressed("shot"))
		{
			m_prevChargeFrame++;

			//チャージ音を鳴らしていなければならす
			if (m_prevChargeFrame >= charge_se_start && !m_isChargeSound)
			{
				SoundManager::GetInstance().Play(SoundType::PlayerCharge);
				m_isChargeSound = true;
			}

			if (m_prevChargeFrame > judg_charge_frame)
			{
				m_isCharging = true;
			}
			if (!m_isCharged)
			{
				//チャージ中アニメーションのUpdate
				m_chargeAnim.Update();
			}
#if _DEBUG
			DrawFormatString(0, 230, 0xffffff, "チャージ中！");
#endif
		}
		if (input.IsReleased("shot"))
		{
			if (m_prevChargeFrame > prev_charge_time &&
				m_shotCooltime < 0)
			{
				//チャージショット
				ChargeShot(pBullets);
				m_isCharging = false;

				//完了アニメーションを停止する
				m_isCharging = false;
				m_isCharged = false;
			}
			else if (m_prevChargeFrame < prev_charge_time &&
				m_weaponType == WeaponType::Normal &&
				m_shotCooltime < 0)
			{
				//通常ショット
				Shot(pBullets);

				m_isCharging = false;
				m_isCharged = false;
			}
			else if (m_prevChargeFrame < prev_charge_time &&
				m_weaponType == WeaponType::Fire &&
				m_shotCooltime < 0)
			{
				//火炎放射
				FireShot(pBullets);
				m_state = PlayerState::Fire;


				m_isCharging = false;
				m_isCharged = false;
			}
			m_shotCooltime = shot_cooltime;
			m_prevChargeFrame = 0;
			m_isChargeSound = false;
			//チャージ音を停止する
			SoundManager::GetInstance().StopSound(SoundType::PlayerCharge);
		}
	}
}

void Player::OnParry()
{
	m_parryCooltime = parry_cooltime;
	m_iFrameTimer = parry_i_frame;
	m_velocity.x = 0.0f;

	// エフェクトを再生する。
	if (m_effectResourceHandle >= 0)
	{
		m_playingEffectHandle = PlayEffekseer2DEffect(m_effectResourceHandle);
		auto manager = GetEffekseer2DManager();
		manager.Get()->SendTrigger(m_playingEffectHandle, 0);
		assert(m_playingEffectHandle > -1);
	}

	//音を再生する
	SoundManager::GetInstance().Play(SoundType::Parry);
}

void Player::ChangeState(PlayerState state)
{
	m_state = state;
}

void Player::UpdateKnockback()
{
	m_knockackTimer--;

	// 減速量
	constexpr float decel = 1.0f;

	if (m_knockbackDir > 0) {         // 右へノックバック中（velocity.x > 0 から 0 へ）
		m_velocity.x = std::max(0.0f, m_velocity.x - decel);
	}
	else if (m_knockbackDir < 0) {  // 左へノックバック中（velocity.x < 0 から 0 へ）
		m_velocity.x = std::min(0.0f, m_velocity.x + decel);
	}
	else {
		// dir==0 の安全策
		m_velocity.x = 0.0f;
	}
}

void Player::Debug(Input& input)
{
	//デバッグ用の武器タイプ切り替え
	if (input.IsTriggered("changeState(player)"))
	{
		if (m_weaponType == WeaponType::Normal)
		{
			m_weaponType = WeaponType::Fire;
			return;
		}
		if (m_weaponType == WeaponType::Fire)
		{
			m_weaponType = WeaponType::Normal;
			return;
		}
	}
	if (input.IsTriggered("parry"))
	{
		m_parryCooltime = -1;
	}
}

void Player::OnDamage(int dir)
{
	//パリィ中なら処理をスキップする
	if (m_state == PlayerState::Parry) return;

	//チャージ完了をfalseにする
	m_isCharged = false;
	m_isCharging = false;
	m_shotCooltime = shot_cooltime;
	m_prevChargeFrame = 0;
	SoundManager::GetInstance().StopSound(SoundType::PlayerCharge);
	m_isChargeSound = false;

	m_knockbackDir = dir;//ノックバックする方向を決める
	m_knockackTimer = knockback_duration;//ノックバックする時間を決める

	//ノックバックするための方向と速度を代入する
	m_velocity.x = m_knockbackDir * knockback_speed;
	m_velocity.y = knockback_jump;

	//エフェクシアのエフェクトを停止する
	auto manager = GetEffekseer2DManager();
	manager.Get()->SendTrigger(m_playingEffectHandle, 1);

	//プレイヤーのHPが2になったらピンチ時の音を鳴らす
	if (m_hitPoint == pinch_hp)
	{
		SoundManager::GetInstance().Play(SoundType::Pinch);
	}

	if (m_hitPoint < 0) return;//すでにHPが0以下なら処理しない

	//チャージ状態や、火炎放射中だと
	//引き継がれたままダメージを受けるので
	//一度アイドルにリセットする
	m_state = PlayerState::Idle;
	m_state = PlayerState::Damage;//ステートを切り替える

	//HPを減らす
	if (m_hitPoint > 0)
	{
		m_hitPoint--;

		if (m_hitPoint == 0)
		{
			m_pEffectFactory->Create(
				{ m_pos.x, m_pos.y },
				EffectType::playerDeath,
				DeathCharactor::Player
			);

			//死亡音を鳴らす
			SoundManager::GetInstance().Play(SoundType::PlayerDeath);
		}
		else
		{
			//被弾音を鳴らす
			SoundManager::GetInstance().Play(SoundType::PlayerDamage);
		}
	}
}

void Player::OnStart()
{
	//行動可能にする
	m_isCanAction = true;
}

void Player::OnSuccessParry()
{

}

const int Player::GetMaxParryCooltime()
{
	return parry_cooltime;
}

void Player::StopParryEffect(bool kill)
{
	//エフェクシアのエフェクトがプレイされてたら
	if (m_playingEffectHandle >= 0)
	{
		if (kill)
		{
			//エフェクトをストップする
			StopEffekseer2DEffect(m_playingEffectHandle);
		}
		else
		{
			StopEffekseer2DEffect(m_playingEffectHandle);
		}
		m_playingEffectHandle = -1;
	}
}
