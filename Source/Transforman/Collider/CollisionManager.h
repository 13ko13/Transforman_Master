#pragma once
#include <vector>
#include <memory>

//敵種類の配列番号
enum class EnemyNumber
{
	ChargeBoss,//配列の0番目
	ParryBoss,
	FlameBoss,
	PlantBoss,
};

class Player;
class EnemyBase;
class PlayerBullet;
class EnemyBullet;
class Rect;
class Camera;
class CollisionManager
{
public:
	/// <summary>
	/// すべての当たり判定をチェックする
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	/// <param name="pEnemies">エネミーのポインタ</param>
	/// <param name="pPlayerBullets">プレイヤーの弾のポインタ</param>
	/// <param name="pEnemyBullets">敵の弾のポインタ</param>
	static void CheckCollisions(
		std::shared_ptr<Player>& pPlayer,
		std::vector<std::shared_ptr<EnemyBase>>& pEnemies,
		std::vector<std::shared_ptr<PlayerBullet>>& pPlayerBullets,
		std::vector<std::shared_ptr<EnemyBullet>>& pEnemyBullets,
		const Rect& swordRect,
		std::shared_ptr<Camera>& pCamera
	);

private:
	/// <summary>
	/// 敵とプレイヤーの当たり判定をチェックする
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	/// <param name="pEnemy">エネミーのポインタ</param>
	/// <returns></returns>
	static bool CheckCollision(const Player& player, const EnemyBase& enemy);

	/// <summary>
	/// プレイヤーの弾と敵の当たり判定をチェックする
	/// </summary>
	/// <param name="pPlayerBullet">プレイヤーの弾の参照</param>
	/// <param name="pEnemy">エネミーの参照</param>
	/// <returns></returns>
	static bool CheckCollision(const PlayerBullet& bullet,const EnemyBase& enemy);

	/// <summary>
	/// プレイヤーと敵の弾の当たり判定をチェックする
	/// </summary>
	/// <param name="pPlayer">プレイヤーの参照</param>
	/// <param name="pEnemyBullet">敵の弾の参照</param>
	/// <returns></returns>
	static bool CheckCollision(const Player& player, const EnemyBullet& bullet);

	/// <summary>
	/// パリィボスの剣とプレイヤーの当たり判定をチェックする
	/// </summary>
	/// <param name="swordRect">剣の当たり判定参照</param>
	/// <param name="player">プレイヤーの参照</param>
	/// <returns></returns>
	static bool CheckCollision(const Rect& swordRect, const Player& player);

	/// <summary>
	/// 敵の弾とプレイヤーのバリアの当たり判定をチェックする
	/// </summary>
	/// <param name="barriorRect">バリアの当たり判定</param>
	/// <param name="bullet">敵の弾の当たり判定</param>
	/// <returns>true:当たっている,false:当たっていない</returns>
	static bool CheckCollision(const Rect& barriorRect, const EnemyBullet& bullet);

	/// <summary>
	/// 敵の当たり判定とバリアの当たり判定
	/// </summary>
	/// <param name="barriorRect">バリアの矩形</param>
	/// <param name="enemyRect">敵の矩形</param>
	/// <returns>true:当たっている,false:当たっていない</returns>
	static bool CheckCollision(const Rect& barriorRect, const Rect& enemyRect);

	/// <summary>
	/// エネミーリストから死亡しているエネミーを削除する
	/// </summary>
	/// <param name="pEnemies"></param>
	static void RemoveDeadEnemies(std::vector<std::shared_ptr<EnemyBase>>& pEnemies);

	/// <summary>
	/// 敵が死亡してるかを取得する関数
	/// </summary>
	/// <param name="pEnemy">敵のポインタ</param>
	/// <returns>死んでいる:true,死んでいない:false</returns>
	static bool IsEnemyDead(const std::shared_ptr<EnemyBase>& pEnemy);
};

