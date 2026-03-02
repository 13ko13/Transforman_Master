#pragma once
#include <memory>

class Player;
class PlayerHpBar;
class EnemyHpBar;
class ChargeShotBoss;
class ParryGageUI;
class UIManager
{
public:
	UIManager(std::shared_ptr<Player> pPlayer, std::shared_ptr<ChargeShotBoss> pChargeBoss);
	~UIManager();

	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<ChargeShotBoss> pChargeBoss);
	void Draw();

private:
	std::shared_ptr<PlayerHpBar> m_playerHpBar;
	std::shared_ptr<EnemyHpBar> m_enemyHpBar;	
	std::shared_ptr<ParryGageUI> m_parryGageUI;
};

