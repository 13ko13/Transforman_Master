#include "ClearScene.h"
#include "../General/Input.h"
#include "TitleScene.h"
#include <memory>
#include "../Main/Application.h"
#include <DxLib.h>
#include "../SoundManager.h"
#include <cassert>

namespace
{
	//フェードにかかるフレーム数
	constexpr int fade_interval = 70;

	//画像関係
	constexpr float clear_text_size = 1.0f;//クリアシーンのテキスト画像の幅
	constexpr float clear_background_size = 1.0f;//クリアシーンの背景画像の幅
	constexpr int textgraph_srcX = 2816;//テキスト画像の切り取りサイズX

	constexpr int blinking_min_alpha = 100;//Press A Button の最小透明度
	constexpr int blinking_rate = 80;//透明度が変化する度合い
	constexpr int a_button_offset_y = 200;//Aボタン画像のYオフセット
	constexpr float a_button_size = 0.5f;//Aボタン画像の表示サイズ
}

ClearScene::ClearScene(SceneController& controller) :
	Scene(controller)
{
	int handle = -1;

	//クリアシーンの背景画像ハンドルロード
	handle = LoadGraph("img/clear/background.png");
	assert(handle > -1 && "クリアシーンの背景画像");//Nullチェック
	m_handles.push_back(handle);

	//Press A Buttonの画像ハンドルロード
	handle = LoadGraph("img/clear/press_a_button.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	//updateとdrawの関数ポインタに
	//FadeInUpdateと
	//FadeDrawを参照させる
	m_update = &ClearScene::UpdateFadeIn;
	m_draw = &ClearScene::DrawFade;

	//frameにfade中にかかる秒数を代入
	m_frame = fade_interval;//初期化関数呼び出し
	Init();

	//BGMを再生
	SoundManager::GetInstance().Play(SoundType::ClearBgm, true);
}

ClearScene::~ClearScene()
{
	//画像ハンドル解放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
	//BGMを停止
	SoundManager::GetInstance().StopSound(SoundType::ClearBgm);
}

void ClearScene::Init()
{
	//クリアシーン背景初期化
	m_backgroundAnim.Init(
		m_handles[static_cast<int>(handleNumber::clearBackground)],0,
		{ 2034, 1296 },
		1, 8,
		0.8f, true);

	m_clearFonthandle = CreateFontToHandle("Melonano", 80, -1, DX_FONTTYPE_EDGE,-1,5);
	m_shadowClearFontH = CreateFontToHandle("Melonano", 75, -1, DX_FONTTYPE_EDGE,-1,7);
}

void ClearScene::Update(Input& input)
{
	//現在割り当てられているメンバUpdate系関数を実行する
	(this->*m_update)(input);
}

void ClearScene::Draw()
{
	//現在割り当てられているメンバDraw系関数を実行する
	(this->*m_draw)();
}

void ClearScene::UpdateFadeIn(Input&)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();

	//フレームが0以下になったらUpdateとDrawをノーマル状態に切り替える
	m_frame--;
	if (m_frame <= 0)
	{
		m_update = &ClearScene::UpdateNormal;
		m_draw = &ClearScene::DrawNormal;
		m_frame = 0;
		//絶対にreturnする
		return;
	}
}

void ClearScene::UpdateNormal(Input& input)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();

	m_frame++;

	//OKボタンが押されたら関数を
	//切り替えてフェードアウトに入る
	if (input.IsTriggered("ok"))
	{
		m_update = &ClearScene::UpdateFadeOut;
		m_draw = &ClearScene::DrawFade;
		//決定音を再生
		SoundManager::GetInstance().Play(SoundType::Decision);
		//フェードアウトの最初
		m_frame = 0;//念のため

		//絶対return
		return;
	}
}

void ClearScene::UpdateFadeOut(Input&)
{
	//背景アニメーション更新
	m_backgroundAnim.Update();


	//フレームを++してfade_intervalを超えたら
	m_frame++;
	if (m_frame >= fade_interval)
	{
		//ゲームシーンに切り替える
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		//ちゃんとreturn
		return;//大事
	}
}

void ClearScene::DrawFade()
{
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//仮で文字表示
#ifdef _DEBUG
	DrawFormatString(500, 500, 0xffffff, "ClearScene");
#endif // _DEBUG

	//背景と文字を描画
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

void ClearScene::DrawNormal()
{
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//クリアシーンの背景を表示
	m_backgroundAnim.Draw({ static_cast<float>(wsize.w / 2), static_cast<float>(wsize.h / 2) }, false);

	//影を描画
	int textW = GetDrawStringWidthToHandle("GAME", 4, m_shadowClearFontH);
	int textPosX = wsize.w / 2 - textW / 2 + 10;
	int textPosY = wsize.h / 2 - 80;

	DrawStringToHandle(textPosX, textPosY, "GAME", 0x000000, m_shadowClearFontH,0x000000);

	textW = GetDrawStringWidthToHandle("CLEAR", 5, m_shadowClearFontH);
	textPosX = wsize.w / 2 - textW / 2 + 10;
	textPosY = wsize.h / 2 + 10;

	DrawStringToHandle(textPosX, textPosY, "CLEAR", 0x000000, m_shadowClearFontH, 0x000000);


	//文字を描画
	textW = GetDrawStringWidthToHandle("GAME", 4, m_clearFonthandle);
	textPosX = wsize.w /2  - textW / 2;
	textPosY = wsize.h / 2 - 90 ;

	DrawStringToHandle(textPosX, textPosY, "GAME", 0xff00ff, m_clearFonthandle, 0xffffff);

	textW = GetDrawStringWidthToHandle("CLEAR", 5, m_clearFonthandle);
	textPosX = wsize.w / 2 - textW / 2;
	textPosY = wsize.h / 2 ;

	DrawStringToHandle(textPosX, textPosY, "CLEAR", 0xff00ff, m_clearFonthandle, 0xffffff);
	

	int blinkingRate = blinking_min_alpha + blinking_rate * sinf(m_frame * 0.1f);// + 1.0f * 0.5f
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, blinkingRate);//aブレンド
	//「Aボタンでスタート」を表示
	DrawRotaGraph(wsize.w / 2, wsize.h / 2 + a_button_offset_y,
		static_cast<double>(a_button_size), 0.0f, m_handles[static_cast<int>(handleNumber::pressAbutton)], true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない

#ifdef _DEBUG
	DrawPixel(wsize.w / 2, wsize.h / 2, 0xff0000);
#endif // DEBUG
}