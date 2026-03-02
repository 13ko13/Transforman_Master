#pragma once
#include "EnemyBase.h"

class Animation;
class EffectFactory;
class ParryBoss : public EnemyBase
{
public:
	ParryBoss(std::shared_ptr<Map> pMap, std::shared_ptr<EffectFactory> effectfactory);
	~ParryBoss();

	void Init() override;
	void Update(GameContext& ctx) override;
	void Draw(std::shared_ptr<Camera> pCamera) override;

	/// <summary>
	/// プレイヤーが到着したときに呼び出される関数
	/// </summary>
	void OnArrive() override;

	//剣の当たり判定を取得する関数
	const Rect& GetSwordHitBox() const { return m_swordHitBox; }

	/// <summary>
	/// ダメージを受けたときに呼び出される関数
	/// </summary>
	void OnDamage(bool isChargeShot) override;

	/// <summary>
	/// 攻撃をパリィされたときの処理
	/// </summary>
	void OnParried(int dir) override;
private:
	/// <summary>
	/// 攻撃行動を行う関数
	/// </summary>
	void Attack() override;

	void IdleDraw(std::shared_ptr<Camera> pCamera);
	void ParryDraw(std::shared_ptr<Camera> pCamera);
	void JumpingDraw(std::shared_ptr<Camera> pCamera);
	void FallAttackDraw(std::shared_ptr<Camera> pCamera);
	void DamageDraw(std::shared_ptr<Camera> pCamera);
	void IdleUpdate(GameContext& ctx);
	void ParryUpdate(GameContext& ctx);
	void JumpingUpdate(GameContext& ctx);
	void FallAttackUpdate(GameContext& ctx);
	void DamageUpdate(GameContext& ctx);

private:
	enum class State
	{
		None,//何もしない
		Appear,//出現
		Idle,//待機,
		Parry,//パリィ		
		Jumping,//ジャンプ
		FallAttack,//落下攻撃
		Damage,//ダメージ
		
		StateNum
	};
	State m_state;//状態

	Animation m_idleAnim; 
	Animation m_parryAnim;
	Animation m_jumpAnim;
	Animation m_fallAttackAnim;
	Animation m_damageAnim;

	int m_moveCooldown;//次の行動までのフレーム数
	int m_parryCooldown;//パリィ可能までのフレーム数

	int m_jumpFrame;//ジャンプ中のフレーム数
	bool m_isJump;//ジャンプ中かどうか
	bool m_hasParried;//パリィを発動したかどうか
	Vector2 m_playerPrevPos;//プレイヤーの前フレームの位置

	//落下攻撃時の剣の当たり判定
	Rect m_swordHitBox;
};