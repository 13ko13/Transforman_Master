#pragma once
#include <vector>
#include "Utility/Geometry.h"
#include "PosProvider.h"
#include <list>
#include <memory>
#include "DeathEffect.h"

enum class EffectType
{
	rush,//ラッシュエフェクト
	hitWall,//壁衝突エフェクト
	dash,//ダッシュエフェクト
	playerDeath,//プレイヤー死亡エフェクト
	enemyDeath,//敵死亡エフェクト
	hitEnemyBullet,//敵の弾衝突エフェクト
	hitPlayerBullet,//プレイヤーの弾衝突エフェクト
	stanEffect,//チャージショットボスのスタンエフェクト
};

class Effect;
class Camera;
class EffectFactory
{
public:
	EffectFactory(); 
	std::weak_ptr<Effect> Create(const Vector2& pos, EffectType type);
	std::weak_ptr<Effect> Create(const Vector2& pos, EffectType type,bool isTurn);
	std::weak_ptr<Effect> Create(const Vector2& pos, EffectType type,DeathCharactor charactor);
	std::weak_ptr<Effect> CreateFollow(std::shared_ptr<PosProvider> target, EffectType type, const Vector2& offset);
	void Update();
	void Draw(const std::shared_ptr<Camera>& pCamera);

private:
	std::vector<int> m_handles;//エフェクトの画像ハンドル
	std::list<std::shared_ptr<Effect>> m_effects;//エフェクトのリスト
};

