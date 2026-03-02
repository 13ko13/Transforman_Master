#include "UIManager.h"
#include "Objects/Player.h"
#include "Objects/ChargeShotBoss.h"
#include "PlayerHpBar.h"
#include "EnemyHpBar.h"
#include "ParryGageUI.h"

UIManager::UIManager(std::shared_ptr<Player> pPlayer, std::shared_ptr<ChargeShotBoss> pChargeBoss)
{
	const int playerHP = pPlayer->GetHitPoint();
	m_playerHpBar = std::make_shared<PlayerHpBar>(playerHP);

	const int enemyHP = pChargeBoss->GetHitPoint();
	m_enemyHpBar = std::make_shared<EnemyHpBar>(enemyHP);

	const int parryCooltime = pPlayer->GetMaxParryCooltime();
	m_parryGageUI = std::make_shared<ParryGageUI>(parryCooltime);
}

UIManager::~UIManager()
{
}

void UIManager::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<ChargeShotBoss> pChargeBoss)
{
	//プレイヤーのHPバーの更新
	m_playerHpBar->Update(pPlayer);
	//エネミーのHPバーの更新
	m_enemyHpBar->Update(pChargeBoss->GetHitPoint());
	//パリィゲージの更新
	m_parryGageUI->Update(pPlayer->GetParryCooltime());
}

void UIManager::Draw()
{
	//プレイヤーのHPバーの描画
	m_playerHpBar->Draw();
	//エネミーのHPバーの描画
	m_enemyHpBar->Draw();
	//パリィゲージの描画
	m_parryGageUI->Draw();
}