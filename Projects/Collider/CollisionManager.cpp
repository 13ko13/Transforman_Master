#include "CollisionManager.h"
#include "../Objects/PlayerBullet.h"
#include "../Objects/EnemyBullet.h"
#include "../Objects/EnemyBase.h"
#include "../Objects/Player.h"
#include "../Utility/Geometry.h"
#include <DxLib.h>
#include "../Collider/Rect.h"
#include "../Objects/ParryBoss.h"
#include "../Graphics/Camera.h"
#include "../Charactor.h"
#include "../SoundManager.h"

namespace
{
	constexpr int shake_power = 2;//カメラを揺らすときの力
	constexpr int parry_shake_power = 8;//パリィ成功時のカメラを揺らす力
}

void CollisionManager::CheckCollisions(
	std::shared_ptr<Player>& pPlayer,
	std::vector<std::shared_ptr<EnemyBase>>& pEnemies,
	std::vector<std::shared_ptr<PlayerBullet>>& pPlayerBullets,
	std::vector<std::shared_ptr<EnemyBullet>>& pEnemyBullets,
	const Rect& swordRect,
	std::shared_ptr<Camera>& pCamera)
{
	//プレイヤーと敵(今のところ使わない)
	for (auto& enemy : pEnemies)
	{
		//プレイヤーが生きていて、
		//無敵中ではなくて、
		//敵が生きていて
		//プレイヤーと敵がぶつかっているとき
		if (!pPlayer->GetIsDead() &&
			!pPlayer->GetIsInvincible() &&
			!enemy->GetIsDead() &&
			CheckCollision(*pPlayer, *enemy))
		{
			//プレイヤーが右と左どちらから
			//攻撃を受けたかで渡す
			// ノックバックする方向を決める
			int dir = 0;
			bool isRight = false;
			if (pPlayer->GetColRect().GetCenter().x > enemy->GetColRect().GetCenter().x)//プレイヤーが右にいる場合
			{
				//プレイヤーがすでに左を向いている場合は
				//向きは変えずに、プレイヤーが右を向いているなら
				//向きを敵の方向に向ける
				if (pPlayer->GetIsRight())
				{
					//左向いてからノックバックさせる
					isRight = false;
				}
				//プレイヤーのノックバックする方向は右
				dir = 1;
			}
			else//プレイヤーが左にいる
			{
				//プレイヤーがすでに右を向いている場合は
				//向きは変えずに、プレイヤーが左を向いているなら
				//向きを敵の方向に向ける
				if (pPlayer->GetIsRight())
				{
					//右向いてからノックバックさせる
					isRight = true;
				}
				//プレイヤーのノックバックする方向は左
				dir = -1;

				//プレイヤーが死んでいるとき以外画面を揺らす
				if (pPlayer->GetHitPoint() != 0)
				{
					//画面を揺らす
					pCamera->OnImpact(shake_power);
				}

			}
			//プレイヤーが死んでいないならダメージ処理を行う
			if (pPlayer->GetHitPoint() != 0)
			{
				pPlayer->OnDamage(dir);
				//キャラクター共通処理のOnDamageを呼ぶ
				pPlayer->Charactor::OnDamage();
			}
		}
		else
		{
			printfDx("\n");
		}
	}

	//プレイヤーの弾と敵
	for (auto& bullet : pPlayerBullets)
	{
		for (auto& enemy : pEnemies)
		{
			if (bullet->GetIsAlive() &&
				!enemy->GetIsDead() &&
				CheckCollision(*bullet, *enemy))
			{
				//通常時は敵にダメージを与える
				//HPが0になったら敵を消す
				if (enemy->GetHitPoint() > 0)
				{
					enemy->OnDamage(bullet->GetType() == BulletType::Charge);
					enemy->Charactor::OnDamage();
				}
				//弾の存在を消す
				bullet->OnDead();
			}
		}
	}

	//プレイヤーと敵の弾
	for (auto& bullet : pEnemyBullets)
	{
		//プレイヤーが生きていて、
		//無敵中ではなくて、
		//敵弾が生きていて
		//プレイヤーと敵弾がぶつかっているとき
		if (!pPlayer->GetIsDead() &&
			!pPlayer->GetIsInvincible() &&
			bullet->GetIsAlive() &&
			CheckCollision(*pPlayer, *bullet))
		{
			//弾の存在を消す
			bullet->OnDead();
			//プレイヤーにノックバックする方向を伝える
			int dir = 0;
			bool isRight = false;
			for (auto& enemy : pEnemies)
			{
				if (pPlayer->GetColRect().GetCenter().x > enemy->GetColRect().GetCenter().x)//プレイヤーが右にいる場合
				{
					//プレイヤーのノックバックする方向は右
					dir = 1;
					//左向いてからノックバックさせる
					isRight = false;
				}
				else//プレイヤーが左にいる
				{
					//プレイヤーのノックバックする方向は左
					dir = -1;
					//右向いてからノックバックさせる
					isRight = true;
				}
			}
			//ノックバックさせる方向と
			//プレイヤーが向く方向を設定
			pPlayer->SetIsRight(isRight);
			//プレイヤーが死んでいないならダメージ処理を行う
			if (pPlayer->GetHitPoint() != 0)
			{
				pPlayer->OnDamage(dir);
			}

			//プレイヤーが死んでいるとき以外画面を揺らす
			if (pPlayer->GetHitPoint() != 0)
			{
				//画面を揺らす
				pCamera->OnImpact(shake_power);
			}
		}
	}

	//パリィボスの剣とプレイヤー
	if (CheckCollision(swordRect, *pPlayer))
	{
		//プレイヤーにノックバックする方向を伝える
		int dir = 0;
		bool isRight = false;
		for (auto& enemy : pEnemies)
		{
			if (pPlayer->GetColRect().GetCenter().x > enemy->GetColRect().GetCenter().x)//プレイヤーが右にいる場合
			{
				//プレイヤーのノックバックする方向は右
				dir = 1;
				//左向いてからノックバックさせる
				isRight = false;
			}
			else//プレイヤーが左にいる
			{
				//プレイヤーのノックバックする方向は左
				dir = -1;
				//右向いてからノックバックさせる
				isRight = true;
			}
		}
		//ノックバックさせる方向と
		//プレイヤーが向く方向を設定
		pPlayer->SetIsRight(isRight);
		//プレイヤーが死んでいないならダメージ処理を行う
		if (pPlayer->GetHitPoint() != 0)
		{
			pPlayer->OnDamage(dir);
			pPlayer->Charactor::OnDamage();
		}

		//プレイヤーが死んでいるとき以外画面を揺らす
		if (pPlayer->GetHitPoint() != 0)
		{
			//画面を揺らす
			pCamera->OnImpact(shake_power);
		}
	}

	//敵の弾とプレイヤーのバリア
	for (auto& bullet : pEnemyBullets)
	{
		if (bullet->GetIsAlive() &&
			!pPlayer->GetIsDead() &&
			CheckCollision(pPlayer->GetBarriorRect(), *bullet))
		{
			//弾を消す
			bullet->OnDead();

			//画面を揺らす
			pCamera->OnImpact(shake_power);

			//パリィ成功の音
			SoundManager::GetInstance().Play(SoundType::ParrySuccess);
		}
	}

	//敵とバリア
	for (auto& enemy : pEnemies)
	{
		if (!enemy->GetIsDead() &&
			!pPlayer->GetIsDead() &&
			CheckCollision(pPlayer->GetBarriorRect(), enemy->GetColRect()))
		{
			//ノックバックする方向
			int dir = 0;

			//パリィ成功時の関数を呼ぶ
			pPlayer->OnSuccessParry();
			if (pPlayer->GetPos().x > enemy->GetPos().x)//プレイヤーが右にいる場合
			{
				//敵のノックバックする方向は左
				dir = -1;
			}
			else//プレイヤーが左にいる
			{
				//敵のノックバックする方向は右
				dir = 1;
			}
			enemy->OnParried(dir);
			pCamera->OnImpact(parry_shake_power);
			//パリィ成功の音
			SoundManager::GetInstance().Play(SoundType::ParrySuccess);
		}
	}

	//敵が死んだら弾も消す
	for (auto& enemy : pEnemies)
	{
		if (enemy->GetIsDead())
		{
			for (auto& bullet : pEnemyBullets)
			{
				bullet->OnDead();
			}
		}
	}

	RemoveDeadEnemies(pEnemies);
}

bool CollisionManager::CheckCollision(const Player& player, const EnemyBase& enemy)
{
	return player.GetColRect().IsCollRect(enemy.GetColRect());
}

bool CollisionManager::CheckCollision(const PlayerBullet& bullet, const EnemyBase& enemy)
{
	return bullet.GetCircle().IsCollWithRect(enemy.GetColRect());
}

bool CollisionManager::CheckCollision(const Player& player, const EnemyBullet& bullet)
{
	return bullet.GetCircle().IsCollWithRect(player.GetColRect());
}

bool CollisionManager::CheckCollision(const Rect& swordRect, const Player& player)
{
	return swordRect.IsCollRect(player.GetColRect());
}

bool CollisionManager::CheckCollision(const Rect& barriorRect, const EnemyBullet& bullet)
{
	return bullet.GetCircle().IsCollWithRect(barriorRect);
}

bool CollisionManager::CheckCollision(const Rect& barriorRect, const Rect& enemyRect)
{
	return barriorRect.IsCollRect(enemyRect);
}

void CollisionManager::RemoveDeadEnemies(std::vector<std::shared_ptr<EnemyBase>>& pEnemies)
{
	pEnemies.erase(std::remove_if(pEnemies.begin(), pEnemies.end(), IsEnemyDead), pEnemies.end());
}

bool CollisionManager::IsEnemyDead(const std::shared_ptr<EnemyBase>& pEnemy)
{
	return pEnemy->GetIsDead();
}
