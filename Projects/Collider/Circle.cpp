#include "Circle.h"
#include "Rect.h"
#include <cmath>
#include <DxLib.h>
#include "../Graphics/Camera.h"

namespace
{
	constexpr int radius = 0;
	const Vector2 first_pos = { 0.0f,0.0f };
}

Circle::Circle() : 
	m_pos(first_pos),
	m_radius(radius)
{
}

Circle::~Circle()
{
}

void Circle::Draw(std::shared_ptr<Camera> pCamera)
{
	//当たり判定を描画
	DrawCircle(
		static_cast<int>(m_pos.x + pCamera->GetDrawOffset().x),
		static_cast<int>(m_pos.y + pCamera->GetDrawOffset().y),
		m_radius, 0xffffff, 0, 1);
#ifdef _DEBUG
	DrawFormatString(0, 335, 0xffffff, "circlePos        : %f, %f", m_pos.x, m_pos.y);
#endif
}

void Circle::SetPos(const Vector2& pos)
{
	m_pos.x = pos.x;
	m_pos.y = pos.y;
}

const float Circle::GetDistance(Vector2& pos) const
{
	float disX = pos.x - m_pos.x;
	float disY = pos.y - m_pos.y;
	return std::sqrt(disX * disX + disY * disY);
}

const bool Circle::IsCollWithRect(const Rect& rect) const
{
	//矩形が自分(Circle)からみてどこにあるかを見る
	//なので矩形の中心に最も近い点を探す
	float closestX = std::fmax(rect.GetLeft(),
		std::fmin(m_pos .x, rect.GetLeft() + rect.GetWidth()));
	float closestY = std::fmax(rect.GetTop(),
		std::fmin(m_pos.y ,rect.GetTop() + rect.GetHeight()));

	//円の中心とその点の距離を計算
	float dx = m_pos.x - closestX;
	float dy = m_pos.y - closestY;
	float distanceSquared = dx * dx + dy * dy;

	return distanceSquared <= (m_radius * m_radius);
}

