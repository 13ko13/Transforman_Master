#include "GameScene.h"
#include "../Objects/Player.h"
#include "../Objects/PlayerBullet.h"
#include "../Objects/ChargeShotBoss.h"
#include "../Objects/EnemyBullet.h"
#include "../Graphics/Camera.h"
#include "../Collider/CollisionManager.h"
#include "../BackGround.h"
#include "../Stages/Stage.h"
#include "../Map.h"
#include "../GameManager.h"
#include "../Objects/ParryBoss.h"
#include "../UIManager.h"
#include "SceneController.h"
#include "GameoverScene.h"
#include "ClearScene.h"
#include "../Main/Application.h"
#include "../EffectFactory.h"
#include "../SoundManager.h"
#include "TextScene.h"
#include "EffekseerForDXLib.h"
#include <DxLib.h>

namespace
{
	constexpr int p_bullet_max = 10;//プレイヤー弾の最大数
	constexpr int e_bullet_max = 20;//敵弾の最大数
	constexpr int fade_interval = 90;//フェードにかかるフレーム数

	//ステージ数の文字の大きさ
	constexpr int font_size = 80;

	//テキスト
	constexpr int text_fade_interval = 50;//テキストのフェードにかかる時間

	//移動操作画像を出す位置
	const Vector2 move_graph_pos = { 400.0f,500.0f };
	//ジャンプ操作画像を出す位置
	const Vector2 jump_graph_pos = { 900.0f,500.0f };
	//ショット操作画像を出す位置
	const Vector2 shot_graph_pos = { 1800.0f,500.0f };
	//パリィ操作画像を出す位置
	const Vector2 parry_graph_pos = {2000.0f,500.0f };
}

GameScene::GameScene(SceneController& controller, StageType stageType) :
	Scene(controller),
	m_isStageClear(false),
	m_isGameover(false),
	m_isGameClear(false),
	m_isFade(false),
	m_textFadeFrame(0)
{
	//ステージデータのロード
	m_pStage = std::make_shared<Stage>();
	m_pStage->Load(1);
	//マップチップの生成
	m_pMap = std::make_shared<Map>(m_pStage);

	//エフェクトファクトリーの生成
	m_pEffectFactory = std::make_shared<EffectFactory>();

	// プレイヤーの生成
	m_pPlayer = std::make_shared<Player>(m_pMap, m_pEffectFactory);
	// プレイヤーの弾の生成
	m_pPlayerBullets.resize(p_bullet_max);
	for (auto& bullet : m_pPlayerBullets)
	{
		bullet = std::make_shared<PlayerBullet>(m_pEffectFactory, m_pMap);
	}
	// 敵の弾の生成
	m_pEnemyBullets.resize(e_bullet_max);
	for (auto& bullet : m_pEnemyBullets)
	{
		bullet = std::make_shared<EnemyBullet>(m_pEffectFactory, m_pMap);
	}
	// 敵の生成
	//チャージショットボス、パリィボス、火炎放射ボス、植物系ボスの4体
	m_pEnemies.resize(0);
	//チャージショットボスを試しに追加する
	m_pChargeShotBoss = std::make_shared<ChargeShotBoss>(m_pMap, m_pEffectFactory,stageType);
	m_pEnemies.push_back(m_pChargeShotBoss);
	//ToDo:1ステージ完成したらコメント化を解除する
	m_pParryBoss = std::make_shared<ParryBoss>(m_pMap, m_pEffectFactory);
	//m_pEnemies.push_back(m_pParryBoss);

	//キャラクターすべてを配列に入れる
	for (auto& enemy : m_pEnemies)
	{
		m_pCharactors.push_back(enemy);
	}
	m_pCharactors.push_back(m_pPlayer);

	//オブジェクトを継承しているクラスをすべてObjects配列に入れる
	for (auto& charactor : m_pCharactors)
	{
		m_pObjects.push_back(charactor);
	}
	for (auto& bullet : m_pEnemyBullets)
	{
		m_pObjects.push_back(bullet);
	}
	for (auto& bullet : m_pPlayerBullets)
	{
		m_pObjects.push_back(bullet);
	}

	//カメラの生成
	m_pCamera = std::make_shared<Camera>();

	//背景の生成
	m_pBackground = std::make_shared<BackGround>();

	//ゲームマネージャーの生成
	m_pGameManager = std::make_shared<GameManager>();

	//当たり判定マネージャーの生成
	m_pColManager = std::make_shared<CollisionManager>();

	//UIマネージャーの生成
	m_pUIManager = std::make_shared<UIManager>(m_pPlayer, m_pChargeShotBoss);

	//updateとdrawをフェードインパターンに切り替えておく
	m_update = &GameScene::UpdateFadeIn;
	m_draw = &GameScene::DrawFade;

	//フェードにかかるフレーム数を代入
	m_frame = fade_interval;

	Init();

	//ステージによって違うBGMを再生
	if (m_controller.GetStageType() == StageType::Stage1)
	{
		SoundManager::GetInstance().Play(SoundType::GameBgm1, true);
	}
	else if (m_controller.GetStageType() == StageType::Stage2)
	{
		SoundManager::GetInstance().Play(SoundType::GameBgm2, true);
	}
	else if (m_controller.GetStageType() == StageType::Stage3)
	{
		SoundManager::GetInstance().Play(SoundType::GameBgm3, true);
	}

	m_fontHandle = CreateFontToHandle("Melonano", font_size, 0, DX_FONTTYPE_NORMAL);

	char tex[] = "STAGE 1";
	int textW = GetDrawStringWidthToHandle(tex, sizeof(tex), m_fontHandle);

	auto wsize = Application::GetInstance().GetWindowSize();
	int drawPosX = wsize.w / 2 - textW / 2;
	int drawPosY = wsize.h / 2 - font_size / 2;
	//文字の位置を初期化
	m_stageTypeTextPos = { static_cast<float>(drawPosX),static_cast<float>(drawPosY) };

	int handle = -1;
	
	handle = LoadGraph("img/how_to_use/a_jump.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	handle = LoadGraph("img/how_to_use/stick_move.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	handle = LoadGraph("img/how_to_use/x_shot.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	handle = LoadGraph("img/how_to_use/y_parry.png");
	m_handles.push_back(handle);
	assert(handle > -1);
}

GameScene::~GameScene()
{
	//BGMを止める
	if (m_controller.GetStageType() == StageType::Stage2)
	{
		SoundManager::GetInstance().StopSound(SoundType::GameBgm1);
	}
	else if (m_controller.GetStageType() == StageType::Stage3)
	{
		SoundManager::GetInstance().StopSound(SoundType::GameBgm2);
	}
	else if (m_controller.GetStageType() == StageType::Clear)
	{
		SoundManager::GetInstance().StopSound(SoundType::GameBgm3);
	}
}

void GameScene::Init()
{
	// 各オブジェクトの初期化
	for (auto& object : m_pObjects)
	{
		object->Init();
	}
}

void GameScene::Update(Input& input)
{
	//現在割り当てられているメンバUpdate系関数を実行する
	(this->*m_update)(input);
}

void GameScene::Draw()
{
	//割り当てられているDraw系メンバ関数を実行する
	(this->*m_draw)();
}

void GameScene::UpdateFadeIn(Input&)
{
	//フレームが0以下になったらUpdateとDrawの関数ポインタに
	//関数を参照させる
	m_frame--;
	if (m_frame <= 0)
	{
		m_update = &GameScene::UpdateNormal;
		m_draw = &GameScene::DrawNormal;
		//絶対return
		return;
	}
}

void GameScene::UpdateNormal(Input& input)
{
	GameContext ctx{ m_pEnemyBullets,m_pPlayerBullets,m_pPlayer,m_pStage,input,m_pCamera };
	// 各オブジェクトの更新
	for (auto& object : m_pObjects)
	{
		object->Update(ctx);
	}

	m_pColManager->CheckCollisions(m_pPlayer, m_pEnemies, m_pPlayerBullets, m_pEnemyBullets, m_pParryBoss->GetSwordHitBox(), m_pCamera);

	// カメラの更新
	m_pCamera->Update(m_pPlayer, m_pStage);

	//マップチップの更新
	m_pMap->Update();

	//ゲームマネージャー更新
	m_pGameManager->Update(m_pPlayer, m_pStage, m_pCamera, m_pChargeShotBoss, m_pParryBoss);

	//UIマネージャー更新
	m_pUIManager->Update(m_pPlayer, m_pChargeShotBoss);

	//エフェクトファクトリー更新
	m_pEffectFactory->Update();

	if (!m_isFade)
	{
		m_textFadeFrame++;
	}
	else
	{
		m_textFadeFrame--;
	}

	/*std::vector<TextScene::PageDesc> pages = {
		{"img/game/text/move.png"}
	};
	m_controller.PushScene(std::make_shared<TextScene>(m_controller,pages));*/

	//プレイヤーが死んだらフェードアウトに遷移する
	if (m_pPlayer->GetIsDead())
	{
		m_update = &GameScene::UpdateFadeOut;
		m_draw = &GameScene::DrawFade;
		//フェードアウトの最初　念のため
		m_frame = 0;
		m_isStageClear = false;//クリアしていない
		m_isGameover = true;//ゲームオーバーになった
		//絶対にreturnする
		return;
	}
	else if (m_pChargeShotBoss->GetIsDead())
	{
		m_update = &GameScene::UpdateFadeOut;
		m_draw = &GameScene::DrawFade;
		m_isStageClear = true;//クリアしていない
		m_isGameover = false;//ゲームオーバーになった
		//フェードアウトの最初　念のため
		m_frame = 0;
		//絶対にreturnする
		return;
	}

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer2D();
}

void GameScene::UpdateFadeOut(Input& input)
{
	GameContext ctx{ m_pEnemyBullets,m_pPlayerBullets,m_pPlayer,m_pStage,input,m_pCamera };

	//ゲームオーバー時のみプレイヤーを更新する
	if (!m_isStageClear && m_isGameover)
	{
		m_pPlayer->Update(ctx);
	}
	//エフェクトファクトリー更新
	m_pEffectFactory->Update();

	//フレームを++してfade_intervalを超えたら
	m_frame++;
	//プレイヤーがゲームをクリアしたなら
	//クリアシーンに遷移する
	//死んだならゲームオーバーシーンに遷移する
	if (!m_isStageClear && m_isGameover)
	{
		if (m_frame >= fade_interval)
		{
			//BGMをストップさせる
			if (m_controller.GetStageType() == StageType::Stage1)
			{
				SoundManager::GetInstance().StopSound(SoundType::GameBgm1);
			}
			else if (m_controller.GetStageType() == StageType::Stage2)
			{
				SoundManager::GetInstance().StopSound(SoundType::GameBgm2);
			}
			else if (m_controller.GetStageType() == StageType::Stage3)
			{
				SoundManager::GetInstance().StopSound(SoundType::GameBgm3);
			}

			// 念のため、プレイヤー側の再生中エフェクトを完全停止
			if (m_pPlayer) {
				m_pPlayer->StopParryEffect(/*kill=*/true);
			}

			UpdateEffekseer2D();

			//ゲームオーバーシーンに遷移
			m_controller.ChangeScene(std::make_shared<GameoverScene>(m_controller));

			//絶対にreturnする
			return;
		}
	}
	else if (m_isStageClear && !m_isGameover)
	{
		//現在のクリア済みステージにアクセスする
		if (m_frame >= fade_interval)
		{
			switch (m_controller.GetStageType())
			{
			case StageType::Stage1:
				m_controller.OnClearStage1();
				break;
			case StageType::Stage2:
				m_controller.OnCrearStage2();
				break;
			case StageType::Stage3:
				m_controller.OnCrearStage3();
				break;
			case StageType::Clear:
				m_isGameClear = true;
				break;
			}

			//ステージタイプがゲームクリアじゃなければ
			if (m_controller.GetStageType() != StageType::Clear)
			{
				// 念のため、プレイヤー側の再生中エフェクトを完全停止
				if (m_pPlayer) {
					m_pPlayer->StopParryEffect(/*kill=*/true);
				}

				UpdateEffekseer2D();

				//次のステージレベルへ
				m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_controller.GetStageType()));
			}
			else
			{
				// 念のため、プレイヤー側の再生中エフェクトを完全停止
				if (m_pPlayer) {
					m_pPlayer->StopParryEffect(/*kill=*/true);
				}

				UpdateEffekseer2D();

				//ゲームクリアになっていたら
				//クリアシーンへ
				m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));
			}
			
			//絶対にreturnする
			return;
		}
	}
}

void GameScene::DrawFade()
{
	//背景の描画
	m_pBackground->Draw();
	//マップチップの描画
	m_pMap->Draw(*m_pCamera);
	// 各オブジェクトの描画
	for (auto& object : m_pObjects)
	{
		object->Draw(m_pCamera);
	}

	//エフェクトファクトリーの描画
	m_pEffectFactory->Draw(m_pCamera);

	//UIマネージャーの描画
	m_pUIManager->Draw();

	//最初のステージのみ表示する
	if (m_controller.GetStageType() == StageType::Stage1)
	{
		int drawPosX = move_graph_pos.x + m_pCamera->GetDrawOffset().x;
		int drawPosY = move_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//移動操作画像を描画
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Move)],
			true);

		drawPosX = jump_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = jump_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//ジャンプ操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Jump)],
			true);

		drawPosX = shot_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = shot_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//ショット操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Shot)],
			true);

		drawPosX = parry_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = parry_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//パリィ操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Parry)],
			true);
	}
	
	//ウィンドウサイズを変数に保存
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();
	//値の範囲をいったん0.0～1.0にしておくといろいろと扱いやすくなる
	auto rate = static_cast<float>(m_frame) / static_cast<float>(fade_interval);
	//aブレンド
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));//DxLibのAlphaブレンドが0～255
	//画面全体に黒フィルムをかける
	DrawBox(0, 0, wsize.w, wsize.h, 0x000000, true);
	//ブレンドしない
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::DrawNormal()
{
	//ウィンドウサイズを変数に保存
	const auto wsize = Application::GetInstance().GetWindowSize();

	//背景の描画
	m_pBackground->Draw();

	//最初のステージのみ表示する
	if(m_controller.GetStageType() == StageType::Stage1)
	{
		int drawPosX = move_graph_pos.x + m_pCamera->GetDrawOffset().x;
		int drawPosY = move_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//移動操作画像を描画
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Move)],
			true);

		drawPosX = jump_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = jump_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//ジャンプ操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Jump)],
			true);

		drawPosX = shot_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = shot_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//ショット操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Shot)],
			true);

		drawPosX = parry_graph_pos.x + m_pCamera->GetDrawOffset().x;
		drawPosY = parry_graph_pos.y + m_pCamera->GetDrawOffset().y;

		//パリィ操作画像を出す位置
		DrawRotaGraph(
			drawPosX,
			drawPosY,
			0.5, 0.0,
			m_handles[static_cast<int>(HandleNomber::Parry)],
			true);
	}

	//マップチップの描画
	m_pMap->Draw(*m_pCamera);
	// 各オブジェクトの描画
	for (auto& object : m_pObjects)
	{
		object->Draw(m_pCamera);
	}

	//UIマネージャーの描画
	m_pUIManager->Draw();

	//エフェクトファクトリーの描画
	m_pEffectFactory->Draw(m_pCamera);

	//Effekseer描画
	DrawEffekseer2D();

	auto rate = static_cast<float>(m_textFadeFrame) / static_cast<float>(text_fade_interval);
	//aブレンド
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));//DxLibのAlphaブレンドが0～255

	switch (m_controller.GetStageType())
	{
	case StageType::Stage1:
		//ステージ数の描画
		DrawStringToHandle(
			m_stageTypeTextPos.x,
			m_stageTypeTextPos.y,
			"STAGE 1",
			0xff00ff,
			m_fontHandle);

		break;
	case StageType::Stage2:
		//ステージ数の描画
		DrawStringToHandle(
			m_stageTypeTextPos.x,
			m_stageTypeTextPos.y,
			"STAGE 2",
			0xff00ff,
			m_fontHandle);

		break;
	case StageType::Stage3:
		//ステージ数の描画
		DrawStringToHandle(
			m_stageTypeTextPos.x,
			m_stageTypeTextPos.y,
			"STAGE 3",
			0xff00ff,
			m_fontHandle);
		break;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	

	if (rate >= 1)
	{
		m_isFade = true;
	}

}