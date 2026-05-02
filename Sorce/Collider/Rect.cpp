#include "Rect.h"
#include <DxLib.h>
#include <cassert>
#include "../Graphics/Camera.h"

Rect::Rect() :
	m_left(0.0f),
	m_right(0.0f),
	m_top(0.0f),
	m_bottom(0.0f)
{

}

Rect::~Rect()
{

}

void Rect::Draw(unsigned int Color, bool isFill,std::shared_ptr<Camera> pCamera)
{
	//デバッグ用の矩形の描画
	DrawBox(static_cast<int>(
		m_left + pCamera->GetDrawOffset().x),
		static_cast<int>(m_top + pCamera->GetDrawOffset().y),
		static_cast<int>(m_right + pCamera->GetDrawOffset().x),
		static_cast<int>(m_bottom + pCamera->GetDrawOffset().y),
		GetColor(255, 0, 0), false);
}

void Rect::SetLT(float left, float top, float width, float height)
{
	m_left = left;
	m_top = top;
	m_right = left + width;
	m_bottom = top + height;
}

void Rect::SetCenter(float x, float y, float width, float height)
{
	m_left = x - width / 2;
	m_top = y - height / 2;
	m_right = x + width / 2;
	m_bottom = y + height / 2;
}

float Rect::GetWidth() const
{
	assert(m_right >= m_left);//左右の入れ替わりをチェック(矩形の右の方が左に来ていたらおかしいのでassert)
	return m_right - m_left;
}

float Rect::GetHeight() const
{
	assert(m_bottom >= m_top);//上下の入れ替わりをチェック(矩形の下の方が上に来ていたらおかしいのでassert)
	return m_bottom - m_top;
}

Vector2 Rect::GetCenter() const
{
	//中心座標
	float x = (m_left + m_right) / 2;
	float y = (m_top + m_bottom) / 2;

	return Vector2{ x,y };
}

bool Rect::IsCollRect(const Rect& rect) const
{
	//絶対に当たらないパターンをはじいていく
	if (m_right < rect.m_left) return false;
	if (m_left > rect.m_right) return false;
	if (m_top > rect.m_bottom) return false;
	if (m_bottom < rect.m_top) return false;

	//それ以外のパターンは当たっているとする
	return true;
}

void Rect::OnHit(float left, float right, float top, float bottom)
{
	m_left = left;
	m_right = right;
	m_top = top;
	m_bottom = bottom;
}

