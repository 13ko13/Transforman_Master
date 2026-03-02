#pragma once
#include "Object.h"
#include "../Charactor.h"
#include "../General/Input.h"
#include <vector>
#include <memory>
#include "../Collider/Rect.h"
#include "../Graphics/Animation.h"

enum class PlayerState
{
	None,
	Idle,
	Walk,
	Shot,
	Jump,
	ChargeShot,
	Parry,
	Fire,
	Damage,
	Dead,
};

enum class WeaponType
{
	Normal,
	Charge,
	Fire,
};

class PlayerBullet;
class EffectFactory;
class Player : public Charactor, public std::enable_shared_from_this<Player>
{
public:
	Player(std::shared_ptr<Map> pMap,std::shared_ptr<EffectFactory> effectfactory);
	~Player();

	void Init() override;
	void Update(GameContext& ctx) override;
	void Draw(std::shared_ptr<Camera> pCamera) override;

	void OnArriveEnemy();

	/// <summary>
	/// CollisionManagerに呼ばせる
	/// NockBack開始時の処理
	/// </summary>
	/// <param name="dir">ノックバックさせる方向</param>
	void OnDamage(int dir);

	/// <summary>
	/// 現在無敵中かどうかを返す関数
	/// </summary>
	/// <returns>true:無敵中,false:無敵ではない</returns>
	const bool GetIsInvincible() const { return m_isInvincible; };

	/// <summary>
	/// 現在弾をチャージしているかどうか
	/// </summary>
	/// <returns>true:チャージ中,false:チャージしていない</returns>
	const bool GetIsCharging() const { return m_isCharging; }

	/// <summary>
	/// ボス戦開始時に呼ぶ関数
	/// </summary>
	void OnStart();

	/// <summary>
	/// 現在のヒットポイントを取得します。
	/// </summary>
	/// <returns>現在のヒットポイントの値を返します。</returns>
	const int GetHitPoint() const { return m_hitPoint; }

	/// <summary>
	/// バリアの矩形を返す関数
	/// </summary>
	/// <returns>バリアの矩形</returns>
	const Rect GetBarriorRect() const {return m_shieldRect;	}

	/// <summary>
	/// パリィが成功したときの処理
	/// </summary>
	void OnSuccessParry();

	/// <summary>
	/// パリィのクールタイムを取得
	/// </summary>
	/// <returns>現在のパリィのクールタイム</returns>
	const int GetParryCooltime() { return m_parryCooltime; };

	/// <summary>
	/// パリィのクールタイムの最大値を取得
	/// </summary>
	/// <returns>パリィのクールタイムの最大値</returns>
	const int GetMaxParryCooltime();

	void StopParryEffect(bool kill);

private:
	enum class HandleNomber
	{
		PlayerHandle,//プレイヤーアニメーションハンドル
		ChargeHandle,//チャージアニメーションハンドル
		ChargedHandle,//チャージ完了時アニメーションハンドル
	};

	//変数一覧
	std::vector<int> m_handles;
	int m_jumpFrame;//ジャンプ力
	int m_shotCooltime;///ショットのクールタイム管理用
	int m_flameThrowerCT;//火炎放射器のクールタイム管理用
	int m_flameThrowCount;//火炎放射中の時間計測用
	int m_knockackTimer;//ノックバック中のタイム管理用
	int m_blinkingTimer;//無敵中の点滅中のタイム管理用
	int m_parryCooltime;//パリィを打てるまでのクールタイム管理用
	int m_iFrameTimer;//パリィ後の無敵中の時間

	/// <note>ジャンプが可能:true,ジャンプ不可能:false</note>
	bool m_isJumping;///ジャンプしているかどうか
	bool m_isPrevJump;//ジャンプ準備中かどうか
	bool m_isCharging;//チャージしているかどうか
	bool m_isCharged;//チャージが完了しているかどうか
	bool m_isArrive;//ボス部屋に到着したかどうか
	bool m_isInvincible;//ダメージを食らった後の無敵中かどうか
	bool m_isCanAction;//プレイヤーが現在行動可能かどうか
	bool m_isStartDash;//ダッシュを始めたかどうか
	bool m_isChargeSound;//チャージ音を鳴らしているかどうか
	bool m_isJumpSound;//ジャンプ音を鳴らしているかどうか

	int m_prevChargeFrame;//チャージショットのためのフレーム数
	int m_animSrcX;		  //現在のアニメーションの横の切り取り位置
	int m_animSrcY;		  //現在のアニメーションの縦の切り取り位置
	int m_animIdx;			//現在のアニメーションのインデックス

	float m_damageAnimFrame;	//現在のダメージアニメーションフレーム数

	int m_knockbackDir;			//ノックバックする方向

	int m_hitPoint;//現在のHP

	Rect m_shieldRect;//パリィ中のシールドの矩形当たり判定

	PlayerState m_state;
	WeaponType m_weaponType;

	Vector2 m_cameraOffset;//カメラのオフセットをメンバ変数として持つ

	int m_playingEffectHandle = -1;//再生中のエフェクト

	Animation m_chargeAnim;//チャージ中のアニメーション
	Animation m_chargedAnim;//チャージ完了アニメーション


	// エフェクトリソースを読み込む
	// ループで無限再生されるエフェクトは、パーティクル自体の生成数が無限だったり、
	// パーティクルの寿命が無限だったりする
	int m_effectResourceHandle;
private:
	//関数一覧
	/// <summary>
	/// ジャンプ処理
	/// </summary>
	void Jump();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move(Input& input);

	/// <summary>
	/// ショット処理
	/// </summary>
	void Shot(std::vector<std::shared_ptr<PlayerBullet>>& pBullets);

	/// <summary>
	/// チャージショットの処理
	/// </summary>
	/// <param name="pBullet">プレイヤーの弾丸を格納する共有ポインタのベクター</param>
	void ChargeShot(std::vector<std::shared_ptr<PlayerBullet>>& pBullets);

	/// <summary>
	/// 火炎放射をする処理
	/// </summary>
	/// <param name="pBullet">プレイヤーの弾丸を格納する共有ポインタのベクター</param>
	void FireShot(std::vector<std::shared_ptr<PlayerBullet>>& pBullet);

	/// <summary>
	/// ショットを打つ準備の処理
	/// </summary>
	/// <param name="input">イベント名</param>
	/// <param name="pBullets">プレイヤーの弾丸</param>
	void PrevShot(Input& input, std::vector<std::shared_ptr<PlayerBullet>>& pBullets);

	/// <summary>
	/// パリィの処理
	/// </summary>
	void OnParry();

	/// <summary>
	/// Stateを切り替える
	/// </summary>
	/// <param name="state">ステート名</param>
	void ChangeState(PlayerState state);

	/// <summary>
	/// ダメージを食らった時のノックバックモーション
	/// </summary>
	/// <param name="isHIt">当たっている:true,当たっていない:false</param>
	void UpdateKnockback();
	
	/// <summary>
	/// デバッグ時に使う処理
	/// </summary>
	void Debug(Input& input);
};