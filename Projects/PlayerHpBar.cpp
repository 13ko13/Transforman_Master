#include "PlayerHpBar.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	constexpr int graph_width = 60;//HPバーと幅
	constexpr int graph_height = 30;//HPバーと高さ

	//描画位置オフセット
	constexpr int draw_offset_x = 150;
	constexpr int draw_offset_y = 90;

	constexpr int pinch_hp = 2;//ピンチと判断するHP
	constexpr int normal_hp = 3;//通常と判断するHP

	constexpr int normal_hp_offset = 75;//通常HPの描画位置オフセット
	constexpr int next_hp_offset = 25;//次のHPの描画位置オフセット

	constexpr double graph_size = 5.0; //拡大率

	constexpr int hp_1 = 1;//HP1
	constexpr int hp_2 = 2;//HP2
	constexpr int hp_3 = 3;//HP3
	constexpr int hp_4 = 4;//HP4
	constexpr int hp_5 = 5;//HP5
}

PlayerHpBar::PlayerHpBar(int maxHitPoint) :
	UserInterface({0,0}),
	m_currentHP(0),
	m_maxHP(maxHitPoint)
{
	int handle = -1;
	handle = LoadGraph("img/game/Player/HP_Bar_Frame.png");//HPバーの枠
	m_handles.push_back(handle);
	assert(handle >-1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Player/HP_Bar_Heart.png");//HPバーのハート
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Player/HP_Bar_Normal.png");//HPバーの通常カラー
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Player/HP_Bar_Pinch.png");//HPバーのピンチ時カラー
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック

	handle = LoadGraph("img/game/Player/HP_Bar_Empty.png");
	m_handles.push_back(handle);
	assert(handle > -1);//ハンドルnullチェック
	
	//HPを設定
	m_currentHP = m_maxHP;
}

PlayerHpBar::~PlayerHpBar()
{
	//画像の開放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
}

void PlayerHpBar::Update()
{
}

void PlayerHpBar::Update(std::shared_ptr<Player> pPlayer)
{
	m_currentHP = pPlayer->GetHitPoint();//HPを更新
}

void PlayerHpBar::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 200, 0xffffff, "PlayerHitPoint : %d", m_currentHP);
#endif // _DEBUG

	////UIの描画
	
	//HPの枠の描画
	DrawRotaGraph(
		static_cast<int>(m_pos.x + draw_offset_x),
		static_cast<int>(m_pos.y + draw_offset_y),
		graph_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::Frame)],
		true, false);

	//ハートの描画
	DrawRotaGraph(
		static_cast<int>(m_pos.x + draw_offset_x),
		static_cast<int>(m_pos.y + draw_offset_y),
		graph_size, 0.0,
		m_handles[static_cast<int>(HandleNomber::Heart)],
		true, false);

	//HPバーの空の部分を先に描画しておいて
	//その上に現在のHPを描画する
	for (int i = 0; i < m_maxHP; ++i)
	{
		//空HPの描画
		DrawRotaGraph(
			static_cast<int>(m_pos.x + normal_hp_offset + i * next_hp_offset),
			static_cast<int>(m_pos.y + draw_offset_y),
			graph_size, 0.0,
			m_handles[static_cast<int>(HandleNomber::Empty)],
			true, false);
	}

	switch (m_currentHP)
	{
	case hp_1://HPが1の時
	case hp_2://HPが2の時
		for (int i = 0; i < m_currentHP; ++i)
		{
			//ピンチ時のHPの描画
			DrawRotaGraph(
				static_cast<int>(m_pos.x + draw_offset_x + i * next_hp_offset),
				static_cast<int>(m_pos.y + draw_offset_y),
				graph_size, 0.0,
				m_handles[static_cast<int>(HandleNomber::Pinch)],
				true, false);
		}
		break;
	case hp_3://HPが3の時
	case hp_4://HPが4の時
	case hp_5://HPが5の時
		for (int i = 0; i < m_currentHP; ++i)
		{
			//ノーマルHP描画
			DrawRotaGraph(
				static_cast<int>(m_pos.x + normal_hp_offset + i * next_hp_offset),
				static_cast<int>(m_pos.y + draw_offset_y),
				graph_size, 0.0,
				m_handles[static_cast<int>(HandleNomber::Normal)],
				true, false);
		}
		break;
	}
	//HPバーの描画
}
