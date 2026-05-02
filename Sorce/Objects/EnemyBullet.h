#pragma once
#include "Bullet.h"
#include <memory>
#include "../Graphics/Animation.h"

enum class EnemyState
{
	Normal,
	Shotting,
	Dead
};

//プロトタイプ宣言
class ChargeShotBoss;
class Player;
class EnemyBullet : public Bullet
{
public:
	EnemyBullet(std::shared_ptr<EffectFactory> effectFactory, std::shared_ptr<Map> pMap);
	~EnemyBullet();

	void Init() override;
	void Update(GameContext& ctx) override;
	void Draw(std::shared_ptr<Camera> pCamera) override;	

	/// <summary>
	/// 方向ベクトルを設定します。
	/// </summary>
	/// <param name="dir">設定する方向ベクトル。</param>
	void SetDir(const Vector2& dir) { m_dir = dir; }

	/// <summary>
	/// 弾が右を向くかどうかを設定
	/// </summary>
	/// <param name="isRight">右向きかどうか</param>
	void SetIsRight(const bool isRight) { m_isRight = isRight; }

	/// <summary>
	/// 弾が死んだときに呼び出す関数
	/// </summary>
	void OnDead() override;
private:
	//変数
	bool m_isRight;//右向きかどうか

	//エネミーステート
	EnemyState m_state;

	Animation m_bulletAnim;//弾のアニメーション

	//関数
	/// <summary>
	/// 弾自身の移動
	/// </summary>
	void Movement();
};

