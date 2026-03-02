#pragma once
#include "../Utility/Geometry.h"
#include <memory>

class Camera;
class Circle;
/// <summary>
/// 矩形を管理するクラス
/// </summary>
class Rect
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Rect();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Rect();

	/// <summary>
	/// 矩形の描画
	/// </summary>
	/// <param name="Color">矩形の色</param>
	/// <param name="isFill">矩形の中身を塗りつぶすかどうか</param>
	void Draw(unsigned int Color, bool isFill,std::shared_ptr<Camera> pCamera);

	/// <summary>
	/// 左上の座標と幅高さを指定
	/// </summary>
	/// <param name="left">左</param>
	/// <param name="top">上</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void SetLT(float left, float top, float width, float height);

	/// <summary>
	/// 中心座標と幅高さを指定
	/// </summary>
	/// <param name="x">中心のX座標</param>
	/// <param name="y">中心のY座標</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void SetCenter(float x, float y, float width, float height);

	/// <summary>
	/// 矩形の幅を取得
	/// </summary>
	/// <returns>矩形の幅</returns>
	float GetWidth() const;

	/// <summary>
	/// 矩形の高さ
	/// </summary>
	/// <returns>矩形の高さ</returns>
	float GetHeight() const;

	/// <summary>
	/// 矩形の中座標を取得
	/// </summary>
	/// <returns>矩形の中心座標</returns>
	Vector2 GetCenter() const;

	/// <summary>
	/// 矩形の左上のX座標を取得
	/// </summary>
	/// <returns>左上のX座標</returns>
	float GetLeft() const { return m_left; }

	/// <summary>
	/// 矩形の右下のX座標を取得
	/// </summary>
	/// <returns>右下のX座標</returns>
	float GetRight() const { return m_right; }

	/// <summary>
	/// 矩形の左上のY座標
	/// </summary>
	/// <returns>左上のY座標</returns>
	float GetTop() const { return m_top; }

	/// <summary>
	/// 矩形の右下のY座標を取得
	/// </summary>
	/// <returns>右下のY座標</returns>
	float GetBottom() const { return m_bottom; }

	/// <summary>
	/// 矩形同士の当たり判定
	/// </summary>
	/// <param name="rect">矩形情報</param>
	/// <returns>当たってるかどうか</returns>
	bool IsCollRect(const Rect& rect) const;

	/// <summary>
	/// 矩形が衝突されたときにその衝突された
	/// 矩形の情報を記憶する関数
	/// </summary>
	/// <param name="left">矩形の左上X</param>
	/// <param name="right">矩形の右上X</param>
	/// <param name="top">矩形の左上Y</param>
	/// <param name="bottom">矩形の右下Y</param>
	void OnHit(float left,float right, float top,float bottom);

public:
	float m_left;  //左上のX座標
	float m_right; //右下のX座標
	float m_top;   //左上のY座標
	float m_bottom;//右下のY座標
};

