#include "TitleScene.h"
#include <DxLib.h>
#include "../General/Input.h"
#include "SceneController.h"
#include "../Main/Application.h"
#include "GameScene.h"
#include <assert.h>
#include "../../Dxlib_h/EffekseerForDXLib.h"
#include "../SoundManager.h"
#include "../StageType.h"

namespace
{
	//フェードにかかるフレーム数
	constexpr int fade_interval = 60;//フェードにかかるフレーム数
	constexpr int a_button_offset_y = 200;//Aボタン画像のYオフセット
	constexpr float title_logo_size = 0.8f;//タイトルロゴの表示サイズ
	constexpr float a_button_size = 0.8f;//Aボタン画像の表示サイズ
	constexpr float effect_pos_x = 500.0f;
	constexpr float effect_pos_y = 500.0f;

	constexpr int blinking_min_alpha = 100;//Press A Button の最小透明度
	constexpr int blinking_rate = 80;//透明度が変化する度合い

	constexpr int logo_pos_offset_y = 150.0f;

	//プレイヤー
	constexpr int p_graph_width = 40;							//画像の横切り取りサイズ
	constexpr int p_graph_height = 40;						//画像の縦切り取りサイズ
	constexpr int p_max_anim_num = 5;						//アニメーションの最大枚数
	constexpr int p_one_anim_frame = 8;						//アニメーションの1枚を見せる長さ
	constexpr float p_draw_scale = 15.0f;					//描画サイズ
	//敵
	constexpr int e_graph_width = 48;							//画像の横切り取りサイズ
	constexpr int e_graph_height = 32;						//画像の縦切り取りサイズ
	constexpr int e_max_anim_num = 4;						//アニメーションの最大枚数
	constexpr int e_one_anim_frame = 10;						//アニメーションの1枚を見せる長さ
	constexpr float e_draw_scale = 15.0f;					//描画サイズ

	//プレイヤー
	constexpr float p_draw_window_rate_x = 0.27f;			//プレイヤーを描画する位置がウィンドウの幅の何割当たりか
	constexpr float p_draw_window_rate_y = 0.75f;			//プレイヤーを描画する位置がウィンドウの高さの何割に当たりか

	//電気
	constexpr float s_draw_window_rate_y = 0.75f;			//雷を描画する位置がウィンドウサイズの高さ何割に当たるか

	//敵
	constexpr float e_draw_window_rate_x = 0.73f;			//敵を描画する位置がウィンドウの幅の何割当たりか
	constexpr float e_draw_window_rate_y = 0.75f;			//敵を描画する位置がウィンドウの高さの何割当たりか

	//プレイヤーと敵の間の雷
	constexpr int s_graph_width = 256;						//雷の画像の幅
	constexpr int s_graph_height = 128;						//雷の画像の高さ
	constexpr int s_max_anim_num = 4;						//雷のアニメーションの最大枚数
	constexpr int s_one_anim_frame = 6;						//雷のアニメーションの1コマを見せる時間
	constexpr float s_draw_scale = 2.0f;					//雷の描画サイズ
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller)
{
	//タイトルロゴハンドルに画像のハンドルを渡す
	int handle = -1;

	handle = LoadGraph("img/title/title_logo_2.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	handle = LoadGraph("img/title/press_a_button.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	handle = LoadGraph("img/title/background.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	handle = LoadGraph("img/title/player.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	handle = LoadGraph("img/title/enemy.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	handle = LoadGraph("img/title/spark.png");
	assert(handle > -1);	//Nullチェック
	m_handles.push_back(handle);

	//updateとdrawの関数ポインタにFadeInUpdateと
	//FadeDrawを参照させる
	m_update = &TitleScene::UpdateFadeIn;
	m_draw = &TitleScene::DrawFade;

	//frameにfadeにかかる秒数を代入
	m_frame = fade_interval;

	// エフェクトリソースを読み込む
	// ループで無限再生されるエフェクトは、パーティクル自体の生成数が無限だったり、
	// パーティクルの寿命が無限だったりする
	/*effectResourceHandle = LoadEffekseerEffect("Effekseer_Effect/barrior.efk");
	assert(effectResourceHandle > -1);*/

	//BGMを再生
	SoundManager::GetInstance().Play(SoundType::TitleBgm,true);

	//プレイヤーアニメーションを初期化
	m_playerAnim.Init(
		m_handles[static_cast<int>(TitleScene::HandleNumber::Player)],
		0,
		{ p_graph_width,p_graph_height },
		p_max_anim_num, p_one_anim_frame,
		p_draw_scale, true);

	//敵アニメーションを初期化
	m_enemyAnim.Init(
		m_handles[static_cast<int>(TitleScene::HandleNumber::Enemy)],
		0,
		{ e_graph_width,e_graph_height },
		e_max_anim_num, e_one_anim_frame,
		e_draw_scale, true);

	//電気のアニメーションを初期化
	m_sparkAnim.Init(
		m_handles[static_cast<int>(TitleScene::HandleNumber::Spark)],
		0,
		{ s_graph_width ,s_graph_height },
		s_max_anim_num, s_one_anim_frame,
		s_draw_scale, true);

	//タイトルに来ると進捗をリセットする
	m_controller.OnReset();
}

TitleScene::~TitleScene()
{
	//画像ハンドル解放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
	// エフェクトリソースを削除する。(Effekseer終了時に破棄されるので削除しなくてもいい)
	/*DeleteEffekseerEffect(effectResourceHandle);*/
	m_controller.Init();

	SoundManager::GetInstance().StopSound(SoundType::TitleBgm);
} 

void TitleScene::UpdateFadeIn(Input&)
{
	//プレイヤーのアニメーションの更新
	m_playerAnim.Update();
	//敵のアニメーションの更新
	m_enemyAnim.Update();
	//雷のアニメーションの更新
	m_sparkAnim.Update();

	//フレームが0以下になったらUpdateとDrawの関数ポインタに
	//関数を参照させる
	if (--m_frame <= 0)
	{
		m_update = &TitleScene::UpdateNormal;
		m_draw = &TitleScene::DrawNormal;
		m_frame = 0;
		//絶対にreturnをする
		return;
	}
}

void TitleScene::UpdateNormal(Input& input)
{
	//プレイヤーのアニメーションの更新
	m_playerAnim.Update();
	//敵のアニメーションの更新
	m_enemyAnim.Update();
	//雷のアニメーションの更新
	m_sparkAnim.Update();

	m_frame++;
	//okボタンが押されたら
	//関数を切り替えてフェードアウトに入る
	if (input.IsTriggered("ok"))
	{
		SoundManager::GetInstance().Play(SoundType::Decision);

		m_update = &TitleScene::UpdateFadeOut;
		m_draw = &TitleScene::DrawFade;
		//フェードアウトの最初　念のため　
		m_frame = 0;
		//絶対にreturnする
		return;
	}


	// 再生、生成停止、完全停止
	//switch (effectState)
	//{
	//case EffectState::Stoped:
	//	// 1で再生
	//	if (CheckHitKey(KEY_INPUT_1) && playingEffectHandle < 0)
	//	{
	//		// エフェクトを再生する。
	//		playingEffectHandle = PlayEffekseer2DEffect(effectResourceHandle);
	//	}
	//	break;
	//case EffectState::Shooting:
	//	// 3で完全停止
	//	if (CheckHitKey(KEY_INPUT_3) && playingEffectHandle >= 0)
	//	{
	//		StopEffekseer2DEffect(playingEffectHandle);
	//		playingEffectHandle = -1;
	//		effectState = EffectState::Stoped;
	//	}
	//	break;
	//default:
	//	break;
	//}

		//------------------------------//
		// エフェクトルーチン
		//------------------------------//
	//if (playingEffectHandle >= 0) // 再生中エフェクトのハンドルがあれば.
	//{
	//	// 再生中のエフェクトを移動
	//	SetPosPlayingEffekseer2DEffect(playingEffectHandle, effect_pos_x, effect_pos_x, 0);

	//	// Effekseerにより再生中のエフェクトを更新する。
	//	UpdateEffekseer2D();
	//}
}

void TitleScene::UpdateFadeOut(Input&)
{
	//プレイヤーのアニメーションの更新
	m_playerAnim.Update();
	//敵のアニメーションの更新
	m_enemyAnim.Update();
	//雷のアニメーションの更新
	m_sparkAnim.Update();

	//フレームを++してfade_intervalを超えたら
	if (++m_frame >= fade_interval)
	{
		//ゲームシーンに切り替える
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller,StageType::Stage1));
		//ちゃんとreturnする
		return;//大事
	}
}

void TitleScene::DrawFade()
{
	//ウィンドウサイズを変数に保存
	const auto& wsize = Application::GetInstance().GetWindowSize();
	DrawNormal();

	//値の範囲をいったん0.0～1.0にしておくといろいろと扱いやすくなる
	auto fadeRate = (static_cast<float>(m_frame) / static_cast<float>(fade_interval));
	//aブレンド
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * fadeRate));//DxLibのAlphaブレンドが0～255
	//画面全体に黒フィルムをかける
	DrawBox(0, 0, wsize.w, wsize.h, 0x000000, true);
	//ブレンドしない
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::DrawNormal()
{
	//ウィンドウサイズを変数に保存
	const auto wsize = Application::GetInstance().GetWindowSize();

	//背景を表示
	DrawExtendGraph(
		0, 0,
		wsize.w, wsize.h, 
		m_handles[static_cast<int>(TitleScene::HandleNumber::Background)], 
		true);

	//ロゴを表示
	DrawRotaGraph(
		wsize.w / 2, wsize.h / 2 - logo_pos_offset_y,
		static_cast<double>(title_logo_size),
		0.0f, 
		m_handles[static_cast<int>(TitleScene::HandleNumber::TitleLogo)],
		true);

	//プレイヤーを表示
	m_playerAnim.Draw(
		{ static_cast<float>(wsize.w * p_draw_window_rate_x),
		static_cast<float>(wsize.h * p_draw_window_rate_y) },
		false);

	//敵を表示
	m_enemyAnim.Draw(
		{ static_cast<float>(wsize.w * e_draw_window_rate_x),
		static_cast<float>(wsize.h * e_draw_window_rate_y) },
		true);

	//雷を表示
	m_sparkAnim.Draw(
		{ static_cast<float>(wsize.w / 2),
		static_cast<float>(wsize.h * s_draw_window_rate_y) },
		false);

	int blinkingRate = blinking_min_alpha + blinking_rate * sinf(m_frame * 0.1f);// + 1.0f * 0.5f
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,blinkingRate);//aブレンド
	//「Aボタンでスタート」を表示
	DrawRotaGraph(
		wsize.w / 2, wsize.h / 2,
		static_cast<double>(a_button_size), 0.0f,
		m_handles[static_cast<int>(TitleScene::HandleNumber::Abutton)], true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

void TitleScene::Init()
{

}

void TitleScene::Update(Input& input)
{
	//現在割り当てられているメンバUpdate系関数を実行する
	(this->*m_update)(input);
}

void TitleScene::Draw()
{
	//割り当てられているDraw系メンバ関数を実行する
	(this->*m_draw)();
}