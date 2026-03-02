#pragma once
#include "Object.h"
#include "../Charactor.h"
#include <memory>
#include "../Collider/Rect.h"
#include <vector>
#include "../Graphics/Animation.h"

// プロトタイプ宣言
class Player;
struct GameContext;
class EffectFactory;
/// <summary>
/// エネミー全体の基底クラス
/// </summary>
class EnemyBase : public Charactor
{
public:
	EnemyBase(int width,int height,std::shared_ptr<Map> pMap,
		std::shared_ptr<EffectFactory> effectfactory);
	virtual ~EnemyBase();

	virtual void Init() override;
	virtual void Update(GameContext& ctx) override;
	virtual void Draw(std::shared_ptr<Camera> pCamera) override;

	virtual void Attack() abstract;
	/// <summary>
	/// プレイヤーがボス部屋に到着したときに呼ばれる関数
	/// </summary>
	virtual void OnArrive() abstract;
	/// <summary>
	/// ダメージを受けたときに呼び出される関数
	/// </summary>
	virtual void OnDamage(bool isChargeShot) abstract;

	/// <summary>
	/// エネミーの体力を取得する関数
	/// </summary>
	/// <returns>体力</returns>
	virtual int GetHitPoint() const { return m_hitPoint; }

	/// <summary>
	/// 攻撃をパリィされたときの処理
	/// </summary>
	virtual void OnParried(int dir) abstract;
private:

protected:
	int m_appearTime;//出現中のタイム計測
	bool m_isAppear;//自分が出現しているかどうか

	Animation m_idleAnim;
	int m_hitPoint;//体力
};

