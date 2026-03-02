#include "GameManager.h"
#include "Objects/Player.h"
#include "General/GameConstants.h"
#include "Graphics/Camera.h"
#include "Objects/ChargeShotBoss.h"
#include "Stages/Stage.h"
#include "Objects/ParryBoss.h"

namespace
{
	int mapchip_size = 32;//マップチップのサイズ
}

GameManager::GameManager() :
	m_isArrive(false),
	m_isAppear(false),
	m_stageType(StageType::Stage1)
{
}

GameManager::~GameManager()
{
}

void GameManager::Update(std::shared_ptr<Player> pPlayer,
	std::shared_ptr<Stage> pStage,
	std::shared_ptr<Camera> pCamera,
	std::shared_ptr<ChargeShotBoss> pChargeBoss,
	std::shared_ptr<ParryBoss> pParryBoss)
{
	//ステージタイプによって更新処理を分岐させる(仮)
	switch (m_stageType)
	{
	case GameManager::StageType::Stage1:
		ChargeBossUpdate(pPlayer, pStage, pCamera, pChargeBoss);
		break;
	case GameManager::StageType::Stage2:
		ParryBossUpdate(pPlayer, pStage, pCamera, pParryBoss);
		break;
	case GameManager::StageType::Stage3:
		break;
	case GameManager::StageType::BossStage:
		break;
	case GameManager::StageType::StageMax:
		break;
	default:
		break;
	}

	//ボスの出現演出を行う(一度だけ)
	//画面の半分
	const int screenHalfW = Graphic::screen_width / 2;//640
	//ステージの横幅から画面の横半分を引いたもの
	const int cameraEndPosX = pStage->GetMapSize().w * mapchip_size - screenHalfW - 1;
	//カメラのポジションがステージ端に来たら
	const bool isCameraArrive = pCamera->GetPos().x >= cameraEndPosX;
	if (!m_isAppear && isCameraArrive)
	{
		pChargeBoss->OnArrive();
		m_isAppear = true;
	}

}

void GameManager::ChargeBossUpdate(std::shared_ptr<Player> pPlayer,
	std::shared_ptr<Stage> pStage,
	std::shared_ptr<Camera> pCamera,
	std::shared_ptr<ChargeShotBoss> pChargeBoss)
{
	//プレイヤーがボスの場所に到着したら
	//isArriveをtrueにする
	if (pPlayer->GetPos().x > Graphic::screen_width)
	{
		m_isArrive = true;
		pCamera->OnArriveEnemy(pPlayer, pStage);

		//到着して、ボスが出現中の間はプレイヤーは行動不能にする
		bool isStart = pChargeBoss->GetIsStart();
		if (isStart)
		{
			pPlayer->OnStart();
		}
		else
		{
			if (pChargeBoss->GetState() == 1)
			{
				pPlayer->OnArriveEnemy();
			}
		}
	}
}

void GameManager::ParryBossUpdate(std::shared_ptr<Player> pPlayer,
	std::shared_ptr<Stage> pStage,
	std::shared_ptr<Camera> pCamera,
	std::shared_ptr<ParryBoss> pParryBoss)
{
}

bool GameManager::ShouldGoNextStage() const
{
	return false;
}

void GameManager::OnStageLoaded(int stageID, std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage, std::shared_ptr<Camera> pCamera)
{
}