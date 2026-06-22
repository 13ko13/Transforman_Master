#include "Geometry.h"
#include <DxLib.h>
#include <cmath>
/// <summary>
/// ベクトルの大きさを計算する
/// </summary>
/// <returns>ベクトルの大きさ</returns>
float Vector2::Length() const
{
	return std::hypot(x, y);
}

void Vector2::Normalize()
{
	auto len = Length();
	if (len == 0.0f)
	{
		//0除算を避ける
		x = 0.0f;
		y = 0.0f;
		return;
	}

	//xとyをlenで割ることで正規化する
	x /= len;
	y /= len;
}

Vector2 Vector2::Normalized() const
{
	auto len = Length();
	if (len == 0.0f)
	{
		//0除算を避ける
		return { 0.0f,0.0f };
	}
	return { x / len, y / len };
}

Vector2 Vector2::operator-() const
{
	/// ベクトルを反転したベクトルを返す
	return { -x,-y };
}

void Vector2::operator+=(const Vector2& val)
{
	/// ベクトルの加算
	x += val.x;
	y += val.y;
}

void Vector2::operator-=(const Vector2& val)
{
	/// ベクトルの減算
	x -= val.x;
	y -= val.y;
}

void Vector2::operator*=(const float scale)
{
	/// ベクトルのスカラー倍
	x *= scale;
	y *= scale;
}

Vector2 Vector2::operator+(const Vector2& val) const
{
	/// ベクトルの加算
	return { x + val.x, y + val.y };
}

Vector2 Vector2::operator-(const Vector2& val) const
{
	/// ベクトルの減算
	return { x - val.x, y - val.y };
}

Vector2 Vector2::operator*(float scale) const
{
	/// ベクトルのスカラー倍
	return { x * scale, y * scale };
}

bool Vector2::operator!=(const Vector2& val) const
{
	/// ベクトルの不等価比較
	return (x != val.x || y != val.y);
}

bool Vector2::operator==(const Vector2& val) const
{
	return (x == val.x && y == val.y);
}
