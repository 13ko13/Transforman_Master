#pragma once
#include "EnemyBase.h"
#include <vector>
#include <memory>
#include "../StageType.h"

enum class HandleNomber
{
	ChargeShotBoss,//ボス自身の画像
	Charge,//チャージ中
	NozzleFlash,//発射口の光
};

class EnemyBullet;
class Player;
class EffectFactory;
class Effect;
class ChargeShotBoss : public EnemyBase,
	public std::enable_shared_from_this<ChargeShotBoss>
{
public:
	ChargeShotBoss(std::shared_ptr<Map> pMap,
		std::shared_ptr<EffectFactory> effectfactory,
		StageType& stagaType);
	~ChargeShotBoss();

	void Init() override;
	void Update(GameContext& ctx) override;
	void Draw(std::shared_ptr<Camera> pCamera) override;
	
	/// <summary>
	/// プレイヤーが到着したときに呼び出される関数
	/// </summary>
	void OnArrive() override;

	/// <summary>
	/// ボス戦が始まっているかどうかを返す関数
	/// </summary>
	/// <returns>true:始まっている,false:始まっていない</returns>
	bool GetIsStart() { return m_isStart; }

	/// <summary>
	/// 現在のチャージショットボスのステートを取得する
	/// </summary>
	/// <returns>チャージショットボスのステート</returns>
	const int GetState() const { return static_cast<int>(m_state); }

	/// <summary>
	/// ダメージを受けたときに呼ぶ関数
	/// </summary>
	void OnDamage(bool isChargeShot) override;

	/// <summary>
	/// 攻撃をパリィされたときの処理
	/// </summary>
	void OnParried(int dir) override;
private:
	std::vector<int> m_handles;//ハンドルの配列

	int m_knockbackDir;//ノックバックする方向
	int m_knockackTimer;//残りノックバック時間
	int m_groundNum;//地面に触れた回数
	int m_prevRushTime;//突進準備中の現在のフレーム数
	int m_shakingTimer;//スタン中の微量の揺れの時間
	int m_nextEffectTimer;//次のエフェクトを出すまでのタイマー

	bool m_isRushing;//突進中かどうか
	int m_actionCooldown;//次の行動までのフレーム数
	bool m_isStart;//ボス戦が始まっているかどうか
	bool m_isPlayingFlash;//ノズルフラッシュを再生しているかどうか

	Vector2 m_drawOffset;//描画のみずらす
	StageType m_stageType;//ステージタイプ

	std::weak_ptr<Effect> m_rushEffect;//突進エフェクト

	Animation m_rushAnim;//突進中のアニメーション
	Animation m_prevRushAnim;//突進準備中のアニメーション
	Animation m_shotAnim;//弾発射中のアニメーション
	Animation m_chargeAnim;//チャージ中のアニメーション
	Animation m_flashAnim;//発射口の光

	enum class State
	{
		None,//何もしない
		Appear,//出現
		Idle,//待機
		PrevRush,//突進準備
		Rush,//突進
		Shot,//弾
		Knockback,//ノックバック
		Stan,//怯み
		Death,//死亡

		MaxState,//ステートの数
	};
	State m_state;//状態

private:
	void Attack() override;
	/// <summary>
	/// 敵の行動を決める関数
	/// </summary>
	/// <param name="pBullets">敵弾の配列のポインタ</param>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	void Attack(std::vector<std::shared_ptr<EnemyBullet>>& pBullets,
		std::shared_ptr<Player> pPlayer);
	/// <summary>
	/// 敵が弾を撃ったときの関数
	/// </summary>
	/// <param name="pBullets"></param>
	/// <param name="pPlayer"></param>
	void ShotUpdate(std::vector<std::shared_ptr<EnemyBullet>>& pBullets,
		std::shared_ptr<Player> pPlayer);
	/// <summary>
	/// 敵が突進した時の関数
	/// </summary>
	/// <param name="ctx"></param>
	void RushUpdate(GameContext& ctx);
	/// <summary>
	/// 突進準備中の関数
	/// </summary>
	/// <param name="ctx"></param>
	void PrevRushUpdate(GameContext& ctx);
	/// <summary>
	/// ボス出現中の関数
	/// </summary>
	void AppearUpdate();
	/// <summary>
	/// アイドル状態の関数
	/// </summary>
	/// <param name="ctx">オブジェクトクラスを継承しているオブジェクトの構造体</param>
	void IdleUpdate(GameContext& ctx);

	/// <summary>
	/// ノックバック時の更新処理
	/// </summary>
	void KnockbackUpdate(GameContext& ctx);

	/// <summary>
	/// スタン時の更新処理
	/// </summary>
	void StanUpdate();

	/// <summary>
	/// パリィされて、スタンした瞬間に呼ぶ関数
	/// </summary>
	void OnStan();

	/// <summary>
	/// 殺された後の更新処理
	/// </summary>
	void DeathUpdate();
};

