#pragma once

/// <summary>
/// 2Dベクトルクラス
/// </summary>
struct Vector2
{
	float x, y;
	float Length()const;//ベクトルの大きさ
	void Normalize();//ベクトルの大きさを1にする
	Vector2 Normalized() const;///大きさを１にしたベクトルを返す
	Vector2 operator-()const;///ベクトルを反転したベクトルを返す
	//演算子オーバーロード
	void operator+=(const Vector2& val);///加算
	void operator-=(const Vector2& val);///減算
	void operator*=(const float scale);///ベクトルをn倍する
	Vector2 operator+(const Vector2& val)const;///加算
	Vector2 operator-(const Vector2& val)const;///減算
	Vector2 operator*(float scale)const;///ベクトルの乗算
	bool operator!=(const Vector2& val)const;///不等価比較
	bool operator==(const Vector2& val)const;///等価比較
};

//座標もベクターとして扱う
using Position2 = Vector2;

/// <summary>
/// 2Dのサイズを定義する構造体
/// </summary>
struct Size
{
	int w;///幅
	int h;///高さ
};

