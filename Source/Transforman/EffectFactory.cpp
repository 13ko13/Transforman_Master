#include "EffectFactory.h"
#include "Effect.h"
#include "RushEffect.h"
#include "HitWallEffect.h"
#include "DashEffect.h"
#include "BulletHitEffect.h"
#include "StanEffect.h"
#include "PosProvider.h"
#include <DxLib.h>
#include <cassert>
#include "Graphics/Camera.h"

EffectFactory::EffectFactory()
{
	int handle = -1;

	//エフェクト画像の読み込み
	//ラッシュエフェクト
	handle = LoadGraph("img/game/Effect/chargeboss_rush_effect.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//壁衝突エフェクト
	handle = LoadGraph("img/game/Effect/chargeboss_hit_wall_effect.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//ダッシュエフェクト
	handle = LoadGraph("img/game/Effect/dash_effect.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//プレイヤーの死亡エフェクト
	handle = LoadGraph("img/game/Effect/player_dead_effect.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//敵の死亡エフェクト
	handle = LoadGraph("img/game/Effect/enemy_dead_effect.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//敵の弾衝突時のエフェクト
	handle = LoadGraph("img/game/Effect/hit_enemy_shot.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//プレイヤーの弾衝突時のエフェクト
	handle = LoadGraph("img/game/Effect/hit_player_shot.png");
	assert(handle > -1);
	m_handles.push_back(handle);

	//チャージショットボスのスタンエフェクト
	handle = LoadGraph("img/game/Effect/stan.png");
	assert(handle > -1);
	m_handles.push_back(handle);
}

std::weak_ptr<Effect> EffectFactory::Create(const Vector2& pos, EffectType type)
{
	std::shared_ptr<Effect> effect;//戻り値用エフェクトポインタ

	switch (type)
	{
	case EffectType::rush:
		//ラッシュエフェクトの生成
		effect = std::make_shared<RushEffect>(
			m_handles[static_cast<int>(EffectType::rush)], pos);
		break;
	case EffectType::hitWall:
		//壁衝突エフェクトの生成
		effect = std::make_shared<HitWallEffect>(
			m_handles[static_cast<int>(EffectType::hitWall)], pos);
		break;
	case EffectType::dash:
		//ダッシュエフェクトの生成
		effect = std::make_shared<DashEffect>(
			m_handles[static_cast<int>(EffectType::dash)], pos);
		break;
	case EffectType::hitEnemyBullet:
		//弾の衝突エフェクトの生成
		effect = std::make_shared<BulletHitEffect>(
			m_handles[static_cast<int>(EffectType::hitEnemyBullet)], pos);
		break;
	case EffectType::hitPlayerBullet:
		//弾の衝突エフェクトの生成
		effect = std::make_shared<BulletHitEffect>(
			m_handles[static_cast<int>(EffectType::hitPlayerBullet )], pos);
		break;
	case EffectType::stanEffect:
		effect = std::make_shared<StanEffect>(
			m_handles[static_cast<int>(EffectType::stanEffect)], pos);
	}

	if (effect) m_effects.push_back(effect);
	return effect;
}

std::weak_ptr<Effect> EffectFactory::Create(const Vector2& pos, EffectType type, bool isTurn)
{
	std::shared_ptr<Effect> effect;//戻り値用エフェクトポインタ

	switch (type)
	{
	case EffectType::dash:
		//ダッシュエフェクトの生成
		effect = std::make_shared<DashEffect>(
			m_handles[static_cast<int>(EffectType::dash)], pos, isTurn);
		break;
	}

	if (effect) m_effects.push_back(effect);
	return effect;
}

std::weak_ptr<Effect> EffectFactory::Create(const Vector2& pos, EffectType type, DeathCharactor charactor)
{
	std::shared_ptr<Effect> effect;//戻り値用エフェクトポインタ

	switch (type)
	{
	case EffectType::playerDeath:
		//死亡エフェクトの生成
		effect = std::make_shared<DeathEffect>(
			m_handles[static_cast<int>(EffectType::playerDeath)], pos, DeathCharactor::Player);

		break;
	case EffectType::enemyDeath:
		//死亡エフェクトの生成
		effect = std::make_shared<DeathEffect>(
			m_handles[static_cast<int>(EffectType::enemyDeath)], pos, DeathCharactor::Enemy);
		break;
	}

	if (effect) m_effects.push_back(effect);
	return effect;
}

std::weak_ptr<Effect> EffectFactory::CreateFollow(
	std::shared_ptr<PosProvider> target,
	EffectType type, const Vector2& offset)
{
	std::shared_ptr<Effect> effect;
	switch (type)
	{
	case EffectType::rush:
	{
		//ラッシュエフェクトの生成
		effect = std::make_shared<RushEffect>(
			m_handles[static_cast<int>(EffectType::rush)],
			target, offset);
		break;
	}
	}

	if (effect) m_effects.push_back(effect);
	return effect;
}

void EffectFactory::Update()
{
	for (auto& effect : m_effects)
	{
		effect->Update();
	}
	m_effects.remove_if([](std::shared_ptr<Effect> effect)
		{
			return effect->GetIsDead();
		});
}

void EffectFactory::Draw(const std::shared_ptr<Camera>& pCamera)
{
	for (auto& effect : m_effects)
	{
		effect->Draw(pCamera);
	}
}