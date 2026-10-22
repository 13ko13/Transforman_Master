#include "ParryGageUI.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace
{
	constexpr int graph_width = 48;//画像の横幅
	constexpr int graph_height = 16;//画像の高さ

	constexpr int frame_src_x = graph_width * 4;//枠の切り取り位置X
	constexpr int gage_src_x = 0;//ゲージ本体の切り取り位置X
	constexpr int src_y = 16;//切り取り位置Y

	const Vector2 pos = { 180.0f,160.0f };
	constexpr double draw_size = 4.0;

	constexpr int barrior_graph_width = 24;//バリア画像の横幅
	constexpr int barrior_graph_height = 24;//バリア画像の高さ
	constexpr int barrior_src_x = barrior_graph_width * 13;//バリア画像の切り取り位置X
	constexpr int barrior_src_y = barrior_graph_height * 9;//バリア画像の切り取り位置Y
	constexpr double barrior_draw_size = 2.0;//バリアの描画サイズ
	constexpr float barrior_draw_offset_x = 120.0f;//バリアの描画時のオフセット

	//パリィが発動可能になった時に
	constexpr int shake_power = 3;//揺らす力
	constexpr int shake_frame = 20;//揺らす時間
	constexpr float shake_rate = 1.5f;//揺らすときの速さの割合
	constexpr int shake_range = 2;//揺らす振幅

	//パリィのボタン
	const Vector2 parry_botton_pos = { 80.0f,178.0f };//位置
	constexpr double botton_draw_size = 0.08;//ボタンサイズ
}

ParryGageUI::ParryGageUI(int maxParryCooltime):
	UserInterface(pos),
	m_isMax(true),
	m_isShake(false),
	m_maxGageShakeFrame(0),
	m_frame(0)
{
	m_currentGage = 0;
	m_maxGage = maxParryCooltime;

	int handle = -1;

	//ゲージに使う画像をロード
	handle = LoadGraph("img/game/Player/parry_gage.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	//ゲージの枠の画像をロード
	handle = LoadGraph("img/game/Player/parry_gage.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	//バリアの画像をロード
	handle = LoadGraph("img/game/Player/barrior.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	//Yボタンの画像をロード
	handle = LoadGraph("img/game/Player/parry_button.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	m_drawOffset = { 0.0f,0.0f };
}

ParryGageUI::~ParryGageUI()
{

}

void ParryGageUI::Update(int nowParryGage)
{
	m_currentGage = nowParryGage;
	m_frame++;

	if (m_maxGageShakeFrame > 0)
	{
		m_maxGageShakeFrame--;
		//sin波により揺れる間隔の早さと、振幅を描画オフセットに追加する
		m_drawOffset.x = static_cast<float>(std::sin(static_cast<double>(m_frame * shake_rate)) * shake_range);
		m_drawOffset.y = static_cast<float>(std::sin(static_cast<double>(m_frame * shake_rate)) * shake_range);
	}
	else
	{
		m_drawOffset = { 0.0f,0.0f };
	}
}

void ParryGageUI::Update()
{
	
}

void ParryGageUI::Draw()
{
	//枠の描画
	DrawRectRotaGraph(
		m_pos.x, m_pos.y,
		frame_src_x, src_y,
		graph_width, graph_height,
		draw_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::Frame)], true, false);

	//現在のゲージの割合を計算
	float rate = 1.0f - static_cast<float>(m_currentGage) / static_cast<float>(m_maxGage);
	//割合を矯正
	rate = std::clamp(rate, 0.0f, 1.0f);

	//切り取り位置を割合によって変更する
	int cut_w = static_cast<int>(std::round(graph_width * rate));
	//切り取り位置を矯正
	cut_w = std::clamp(cut_w, 0, graph_width);

	//バリアの描画
	DrawRectRotaGraph(
		m_pos.x - barrior_draw_offset_x, m_pos.y,
		barrior_src_x, barrior_src_y,
		barrior_graph_width, barrior_graph_height,
		barrior_draw_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::Barrior)],
		true, false);

	//ボタンの描画
	DrawRotaGraph(
		parry_botton_pos.x,parry_botton_pos.y,
		botton_draw_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::ParryBotton)],
		true, false);

	if (cut_w <= 0) return;//空なら描画しない

	//切り取り位置を変更するとDrawRectRotaGraphは真ん中基準の描画なので
	//位置がずれてしまうから矯正する
	float drawW_full = graph_width * static_cast<float>(draw_size);//最大の幅
	float drawW_now = cut_w * static_cast<float>(draw_size);//現在の幅

	//最大の描画時の左端座標
	float left = m_pos.x - drawW_full / 2.0f;

	//左端 + 現在幅 / 2 が「中心」
	float x = left + drawW_now / 2;

	if (rate >= 1.0f)
	{
		if (!m_isShake)
		{
			//満タンになった瞬間だけ
			m_maxGageShakeFrame = shake_frame;
			//フラグを立てる
			m_isShake = true;
		}
	}
	else
	{
		//満タンじゃなくなったら解除
		m_isShake = false;
	}

	//ゲージの描画
	DrawRectRotaGraph(
		x + m_drawOffset.x, m_pos.y + m_drawOffset.y,
		gage_src_x, src_y,
		cut_w, graph_height,
		draw_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::Gage)], true, false);
}