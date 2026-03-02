#include "EnemyHpBar.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	constexpr int graph_width = 60;//HPバーと幅
	constexpr int graph_height = 30;//HPバーと高さ

	//描画位置オフセット
	constexpr int draw_frame_x = 870;//枠の位置
	constexpr int draw_frame_y = 10;//枠の位置
	constexpr int draw_empty_offset_x = 75;//空HPのXオフセット
	constexpr int draw_empty_offset_y = 75;//空HPのXオフセット
	constexpr int draw_heart_offset_x = 125;//ハートのXオフセット
	constexpr int draw_heart_offset_y = 75;//ハートのYオフセット
	constexpr int draw_pinch_offset_x = 150;//ピンチHPのXオフセット
	constexpr int draw_pinch_offset_y = 75;//ピンチHPのYオフセット

	constexpr int pinch_hp = 2;//ピンチと判断するHP
	constexpr int normal_hp = 3;//通常と判断するHP

	constexpr int normal_hp_offset = 925;//通常HPの描画位置オフセット
	constexpr int next_hp_offset = 25;//次のHPの描画位置オフセット

	constexpr double graph_size = 5.0; //拡大率

	constexpr int hp_1 = 1;//HP1
	constexpr int hp_2 = 2;//HP2
	constexpr int hp_3 = 3;//HP3
	constexpr int hp_4 = 4;//HP4
	constexpr int hp_5 = 5;//HP5
	constexpr int hp_6 = 6;//HP6
	constexpr int hp_7 = 7;//HP7
	constexpr int hp_8 = 8;//HP8
	constexpr int hp_9 = 9;//HP9
	constexpr int hp_10 = 10;//HP10

	constexpr int first_frame_width = 20;//最初の枠の幅
	constexpr int last_frame_srcX = 40;//最後の枠の切り取り開始位置
	constexpr int last_frame_width = 20;//最後の枠の幅
	constexpr int draw_last_offset_x = 217;//最後の枠の描画位置オフセット
	constexpr int frame_height = 30;//枠の高さ
	constexpr int one_hp_width = 5;//1メモリの幅
	constexpr int empty_hp_offset_x = 15;//空HPの描画位置オフセット
	constexpr int pinch_hp_offset = 15;//ピンチ時HPの描画位置オフセット

	constexpr int hp_per_memory = 3;//HPバーのメモリ1つ当たりのHP
	constexpr int pinch_per = 3;//HPがピンチHPメモリになる割合
}

EnemyHpBar::EnemyHpBar(int maxHitPoint) :
	UserInterface({ 0,0 }),
	m_currentHP(0),
	m_maxHP(maxHitPoint)
{
	int handle = -1;
	handle = LoadGraph("img/game/Enemy/HP_Bar_Frame.png");//HPバーの枠
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Enemy/HP_Bar_Heart.png");//HPバーのハート
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Enemy/HP_Bar_Normal.png");//HPバーの通常カラー
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Enemy/HP_Bar_Pinch.png");//HPバーのピンチ時カラー
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Enemy/HP_Bar_Empty.png");
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	//HPを設定
	m_currentHP = m_maxHP;

	m_pos = { draw_frame_x ,draw_frame_y };
}

EnemyHpBar::~EnemyHpBar()
{
	//画像の開放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
}

void EnemyHpBar::Update()
{
}

void EnemyHpBar::Update(int currentHP)
{
	m_currentHP = currentHP;//HPを更新
}

void EnemyHpBar::Draw()
{
	//UIの描画
	//HPの枠の描画
	DrawRectRotaGraphLT(m_pos.x, m_pos.y,
		0, 0, first_frame_width, frame_height,
		graph_size,
		m_handles[static_cast<int>(HandleNomber::Frame)],
		true);

	//枠の真ん中を表示する位置
	int midFramePos = m_pos.x + first_frame_width * graph_size;
	//HPバーの枠が長くなりすぎてしまうため
	//HPの1メモリを3として扱う
	for (int i = 0; i < m_maxHP / hp_per_memory - 1; i++)
	{
		DrawRectRotaGraphLT(midFramePos + one_hp_width * graph_size * i, m_pos.y,
			first_frame_width, 0,
			one_hp_width, frame_height,
			graph_size,
			m_handles[static_cast<int>(HandleNomber::Frame)],
			true);
	}

	//枠の最後を表示する位置
	int lastFramePos = (midFramePos + one_hp_width * graph_size * m_maxHP / hp_per_memory) - one_hp_width * graph_size;

	DrawRectRotaGraphLT(lastFramePos, m_pos.y,
		last_frame_srcX, 0,
		last_frame_width, frame_height,
		graph_size,
		m_handles[static_cast<int>(HandleNomber::Frame)],
		true);

	//ハートの表示
	DrawRotaGraph(m_pos.x + one_hp_width * graph_size + draw_heart_offset_x, m_pos.y + draw_heart_offset_y,
		graph_size, 0.0, m_handles[static_cast<int>(HandleNomber::Heart)],
		true);

	//空HPの表示
	for (int i = 0; i < m_maxHP / hp_per_memory; i++)
	{
		DrawRotaGraph(m_pos.x + one_hp_width * graph_size * i + draw_empty_offset_x, m_pos.y + draw_empty_offset_y,
			graph_size, 0.0, m_handles[static_cast<int>(HandleNomber::Empty)],
			true);
	}

	//ピンチHP
	//メモリは1本あたり3HPとして扱うので3ずつ増やす
	for (int i = 0; i < m_currentHP; i += hp_per_memory)
	{
		DrawRotaGraph(m_pos.x + one_hp_width * graph_size * i / hp_per_memory + draw_pinch_offset_x, m_pos.y + draw_pinch_offset_y,
			graph_size, 0.0, m_handles[static_cast<int>(HandleNomber::Pinch)],
			true);
	}

	//ノーマルHP
	//ピンチじゃないときだけ表示する
	if (m_currentHP > m_maxHP / pinch_per)//現在HP > 最大HPの3割
	{
		//メモリは1本あたり3HPとして扱うので3ずつ増やす
		for (int i = 0; i < m_currentHP; i += hp_per_memory)
		{
			//現在のHP分描画
			//iは3ずつ増えるので1メモリ分の位置調整をする
			DrawRotaGraph(m_pos.x + one_hp_width * graph_size * i / hp_per_memory + draw_empty_offset_x, m_pos.y + draw_empty_offset_y,
				graph_size, 0.0, m_handles[static_cast<int>(HandleNomber::Normal)],
				true);
		}
	}

#ifdef _DEBUG
	DrawFormatString(0, 215, 0xffffff, "EnemyHitPoint : %d", m_currentHP);
	DrawPixel(m_pos.x, m_pos.y, 0xffaaff);
	DrawPixel(m_pos.x + first_frame_width * 5, m_pos.y, 0xffaaff);

#endif // _DEBUG
}

inline void EnemyHpBar::DrawRectRotaGraphLT(int x, int y, int srcX, int srcY, int srcW, int srcH, double scale, int handle, bool trans)
{
	DrawRectRotaGraph2(
		x, y,
		srcX, srcY, srcW, srcH,
		0.0f, 0.0f,//CenterX,CenterYを0.0に
		scale, 0.0f,//拡大、角度
		handle, trans
	);
}