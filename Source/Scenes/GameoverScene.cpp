#include "GameoverScene.h"
#include "../General/Input.h"
#include "TitleScene.h"
#include <memory>
#include "../Main/Application.h"
#include "../SoundManager.h"
#include <DxLib.h>
#include "GameScene.h"
#include <cassert>
#include <cmath>

namespace
{
	//フェードにかかるフレーム数
	constexpr int fade_interval = 100;

	//画像関係
	constexpr float gameover_text_size = 1.0f;//ゲームオーバーのテキスト画像の幅
	constexpr float gameover_background_size = 1.0f;//ゲームオーバーの背景画像の幅

	constexpr int blinking_min_alpha = 100;//Press A Button の最小透明度
	constexpr int blinking_rate = 80;//透明度が変化する度合い
	constexpr float a_button_size = 0.5f;//Aボタン画像の表示サイズ
	constexpr int a_button_offset_y = 200;//Aボタン画像のYオフセット

	constexpr int shake_power = 2;//スタン時の揺らす力
	constexpr float shake_rate = 0.2f;//揺らすときの速さの割合
	constexpr int shake_range = 2;//揺らす振幅
}

GameoverScene::GameoverScene(SceneController& controller) : 
	Scene(controller)
{
	//ゲームオーバーの文字画像ハンドルロード
	int handle = LoadGraph("img/gameover/gameover_text.png");
	assert(handle > -1);//Nullチェック
	m_handles.push_back(handle);

	//ゲームオーバーの背景画像ハンドルロード
	handle = LoadGraph("img/gameover/Gameover_background.png");
	assert(handle > -1);//Nullチェック
	m_handles.push_back(handle);

	//Press A Button 
	handle = LoadGraph("img/title/press_a_button.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	//updateとdrawの関数ポインタにFadeInUpdateと
	//FadeDrawを参照させる
	m_update = &GameoverScene::UpdateFadeIn;
	m_draw = &GameoverScene::DrawFade;

	//frameにfade中にかかる秒数を代入
	m_frame = fade_interval;

	m_drawOffset = { 0.0f,0.0f };

	Init();

	//BGMを再生
	SoundManager::GetInstance().Play(SoundType::GameoverBgm, true);
}

GameoverScene::~GameoverScene()
{
	//画像ハンドル解放
	for(auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
	//BGMを停止
	SoundManager::GetInstance().StopSound(SoundType::GameoverBgm);
}

void GameoverScene::Init()
{
	//ゲームオーバー背景アニメーション初期化
	m_backgroundAnim.Init(
		m_handles[static_cast<int>(handleNumber::gameoverBackground)],
		0, 
		{1360, 784},
		2,5,
		1.0f,true);
}

void GameoverScene::Update(Input& input)
{
	//現在割り当てられているメンバUpdate系関数を実行する
	(this->*m_update)(input);
}

void GameoverScene::Draw()
{
	//現在割り当てられているメンバDraw系関数を実行する
	(this->*m_draw)();
}

void GameoverScene::UpdateFadeIn(Input&)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();

	//sin波により揺れる間隔の早さと、振幅を描画オフセットに追加する
	m_drawOffset.y = static_cast<float>(std::sin(static_cast<double>(m_frame * shake_rate)) * shake_range);

	//フレームが0以下になったらUpdateとDrawをノーマル状態に切り替える
	m_frame--;
	if (m_frame <= 0)
	{
		m_update = &GameoverScene::UpdateNormal;
		m_draw = &GameoverScene::DrawNormal;
		//絶対にreturnする
		return;
	}
}

void GameoverScene::UpdateNormal(Input& input)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();

	m_frame++;

	//sin波により揺れる間隔の早さと、振幅を描画オフセットに追加する	
	m_drawOffset.y = static_cast<float>(std::sin(static_cast<double>(m_frame * shake_rate)) * shake_range);

	//OKボタンが押されたら関数を
	//切り替えてフェードアウトに入る
	if (input.IsTriggered("ok"))
	{
		m_update = &GameoverScene::UpdateFadeOut;
		m_draw = &GameoverScene::DrawFade;
		//フェードアウトの最初
		m_frame = 0;//念のため

		//決定音を再生
		SoundManager::GetInstance().Play(SoundType::Decision);

		//絶対return
		return;
	}
}

void GameoverScene::UpdateFadeOut(Input&)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();

	//フレームを++してfade_intervalを超えたら
	m_frame++;
	if (m_frame >= fade_interval)
	{
		//ゲームシーンに切り替える
		if (m_controller.GetStageType() == StageType::Stage1)
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_controller.GetStageType()));
		}
		else if (m_controller.GetStageType() == StageType::Stage2)
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_controller.GetStageType()));
		}
		else if (m_controller.GetStageType() == StageType::Stage3)
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_controller.GetStageType()));
		}
		//ちゃんとreturn
		return;//大事
	}
}

void GameoverScene::DrawFade()
{
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//仮で文字表示
#ifdef _DEBUG
	DrawFormatString(500, 500, 0xffffff, "GameoverScene");
#endif // _DEBUG

	//通常の描画を呼ぶ
	DrawNormal();

	//値の範囲をいったん0.0～1.0にしておくといろいろと扱いやすくなる
	auto rate = static_cast<float>(m_frame) / static_cast<float>(fade_interval);
	//aブレンド
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));//DxLibのAlphaブレンドが0～255
	//画面全体に黒フィルムをかける
	DrawBox(0, 0, wsize.w, wsize.h, 0x000000, true);
	//ブレンドしない
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameoverScene::DrawNormal()
{
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();
	
	//ゲームオーバーの背景を表示
	int drawPosX = wsize.w / 2 + m_drawOffset.x;
	int drawPosY = wsize.h / 2 + m_drawOffset.y;
	m_backgroundAnim.Draw({ static_cast<float>(drawPosX), static_cast<float>(drawPosY) }, false);
	//ゲームオーバーのテキストを表示
	DrawRotaGraph(wsize.w / 2, wsize.h / 2, static_cast<double>(gameover_text_size), 0.0f, m_handles[static_cast<int>(handleNumber::gameoverText)], true);

	int blinkingRate = blinking_min_alpha + blinking_rate * sinf(m_frame * 0.1f);// + 1.0f * 0.5f
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, blinkingRate);//aブレンド
	//「Aボタンでスタート」を表示
	DrawRotaGraph(wsize.w / 2, wsize.h / 2 + a_button_offset_y,
		static_cast<double>(a_button_size), 0.0f, m_handles[static_cast<int>(handleNumber::Abutton)], true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}