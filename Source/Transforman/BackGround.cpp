#include "BackGround.h"
#include <DxLib.h>
#include "General/GameConstants.h"
#include <cassert>

namespace
{
	constexpr double graph_scale = 1.0;//âÊëúÇÃägëÂî{ó¶
	constexpr double graph_angle = 0.0;//âÊëúÇÃäpìx
}

BackGround::BackGround() :
	m_pos(0.0f,0.0f)
{
	m_handle = LoadGraph("img/game/Map/background.png");
	//É`ÉFÉbÉN
	assert(m_handle > -1);
}

BackGround::~BackGround()
{
	DeleteGraph(m_handle);
}

void BackGround::Update()
{

}

void BackGround::Draw()
{
	//îwåi
	DrawRotaGraph(static_cast<int>(m_pos.x + Graphic::screen_width / 2),
					static_cast<int>(m_pos.y + Graphic::screen_height / 2),
					graph_scale, graph_angle,
					m_handle, true, false);
}
