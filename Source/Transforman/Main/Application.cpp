#include "Application.h"
#include "DxLib.h"
#include "../General/Input.h"
#include "../Scenes/SceneController.h"
#include <memory>
#include "../Scenes/TitleScene.h"
#include "../General/GameConstants.h"
#include "EffekseerForDXLib.h"

namespace
{
	constexpr int effect_num = 8000;
	constexpr int desition_volume = 170;//決定音の音量
	constexpr int normal_shot_vol = 170;//通常ショットの音量
	constexpr int game_1_bgm_volume = 230;//ゲームシーンBGM1の音量
	constexpr int game_2_bgm_volume = 230;//ゲームシーンBGM2の音量
	constexpr int game_3_bgm_volume = 230;//ゲームシーンBGM3の音量
	constexpr int title_bgm_volume = 230;//タイトルシーンBGMの音量
	constexpr int clear_bgm_volume = 230;//クリアシーンBGMの音量
	constexpr int gameover_bgm_volume = 230;//ゲームオーバーシーンBGMの音量
	constexpr int charge_shot_vol = 170;//チャージショット時のSEの音量
	constexpr int player_charge_vol = 170;//プレイヤーのチャージ音量
	constexpr int player_charge_finished_vol = 170;//プレイヤーのチャージ完了音量
	constexpr int jump_volume = 170;//ジャンプの音量
	constexpr int hit_shot_volume = 170;//弾が当たった時の音量
	constexpr int pinch_volume = 170;//プレイヤーがピンチになった時の音の音量
	constexpr int rush_volume = 170;//敵の突進時の音の音量
	constexpr int thunder_volume = 170;//雷の音量
	constexpr int player_damage_vol = 170;//プレイヤーが被弾したと音の音量
	constexpr int player_death_vol = 170;//プレイヤーが死んだときの音の音量
	constexpr int stan_volume = 170;//スタンの時の音量
	constexpr int enemy_death_vol = 170;//敵の死亡音の音量
	constexpr int parry_success_vol = 170;//パリィが成功したときの音の音量
	constexpr int parry_volume = 255;//パリィを発動したときの音
}

Application::Application():
	m_windowSize{Graphic::screen_width,Graphic::screen_height}
{

}

void Application::SoundLoad(SoundManager& sm)
{
	sm.Load(SoundType::Decision, "Sound/SE/decision.wav", desition_volume, false);
	sm.Load(SoundType::NormalShot, "Sound/SE/normal_shot.wav", normal_shot_vol, false);
	sm.Load(SoundType::GameBgm1, "Sound/BGM/game1.mp3", game_1_bgm_volume, true);
	sm.Load(SoundType::GameBgm2, "Sound/BGM/game2.mp3", game_2_bgm_volume, true);
	sm.Load(SoundType::GameBgm3, "Sound/BGM/game3.mp3", game_3_bgm_volume, true);
	sm.Load(SoundType::TitleBgm, "Sound/BGM/title.mp3", title_bgm_volume, true);
	sm.Load(SoundType::ClearBgm, "Sound/BGM/clear.mp3", clear_bgm_volume, true);
	sm.Load(SoundType::GameoverBgm, "Sound/BGM/gameover.mp3", gameover_bgm_volume, true);
	sm.Load(SoundType::BossShot, "Sound/SE/charge_shot.wav", charge_shot_vol, false);
	sm.Load(SoundType::PlayerCharge, "Sound/SE/player_charge.wav", player_charge_vol, false);
	sm.Load(SoundType::PlayerChargeFinished, "Sound/SE/player_charge_finished.wav", player_charge_finished_vol, false);
	sm.Load(SoundType::PlayerChargeShot, "Sound/SE/charge_shot.wav", charge_shot_vol, false);
	sm.Load(SoundType::Jump, "Sound/SE/jump.mp3", jump_volume, false);
	sm.Load(SoundType::HitShot, "Sound/SE/hit_shot.mp3", hit_shot_volume, false);
	sm.Load(SoundType::Pinch, "Sound/SE/pinch.mp3", hit_shot_volume, false);
	sm.Load(SoundType::Rush, "Sound/SE/rush.mp3", rush_volume, false);
	sm.Load(SoundType::Thunder, "Sound/SE/thunder.wav", thunder_volume, false);
	sm.Load(SoundType::PlayerDamage, "Sound/SE/player_damage.mp3", player_damage_vol, false);
	sm.Load(SoundType::PlayerDeath, "Sound/SE/player_death.mp3", player_death_vol, false);
	sm.Load(SoundType::Stan, "Sound/SE/stan.wav", stan_volume, false);
	sm.Load(SoundType::EnemyDeath, "Sound/SE/enemy_death.wav", enemy_death_vol, false);
	sm.Load(SoundType::ParrySuccess, "Sound/SE/parry_success.mp3", parry_success_vol, false);
	sm.Load(SoundType::Parry, "Sound/SE/parry.wav", parry_volume, false);
}

Application::~Application()
{

}

Application& Application::GetInstance()
{
	//staticで宣言してそれを返す
	static Application instance;
	return instance;
}

bool Application::Init()
{
	SetGraphMode(m_windowSize.w, m_windowSize.h, Graphic::color_bit);
	//ウィンドwモード設定
	ChangeWindowMode(false);
	//ゲーム名
	SetWindowText("TransforMan");

	if (DxLib_Init() == -1)
	{
		return false;
	}

	//エフェクシアを使用する際に必ず設定する
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(effect_num) == -1)
	{
		DxLib_End();
		return -1;
	}

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
		// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Effekseerに2D描画の設定をする。
	Effekseer_Set2DSetting(Graphic::screen_width, Graphic::screen_height);

	// Zバッファを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetWriteZBuffer3D(TRUE);

	AddFontResourceEx("data/Melonano_Ver.1.31.ttf", FR_PRIVATE, NULL);

	//サウンドマネージャーの初期化
	auto& sm = SoundManager::GetInstance();
	SoundLoad(sm);
	return true;
}

void Application::Run()
{
	SetDrawScreen(DX_SCREEN_BACK);
	Input input;//入力のためのオブジェクト 
	SceneController controller;//シーンを管理するオブジェクト
	controller.ChangeScene(std::make_shared<TitleScene>(controller));
	controller.Init();

	while (ProcessMessage() != -1)
	{
		auto startTime = GetNowHiPerformanceCount();

		ClearDrawScreen();
		input.Update();//入力状態の更新

		//シーンの更新
		controller.Update(input);
		//シーンの描画
		controller.Draw();
		ScreenFlip();
		//ESCキーが押されたらウィンドウを閉じる
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		while (GetNowHiPerformanceCount() - startTime < 16667)
		{

		}
	}
}

void Application::Terminate()
{
	RemoveFontResourceEx("data/Melonano_Ver.1.31.ttf", FR_PRIVATE, NULL);
	Effkseer_End();// Effekseerを終了する。
	DxLib_End();
}

const Size& Application::GetWindowSize() const
{
	return m_windowSize;
}
