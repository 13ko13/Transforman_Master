#pragma once
#include "../Utility/Geometry.h"
class Animation
{
public:
	Animation();
	~Animation();
	
	/// <summary>
	/// 普通のアニメーション初期化
	/// </summary>
	/// <param name="drawHandle">画像ハンドル</param>
	/// <param name="animIndex">切り取り位置</param>
	/// <param name="frameSize">アニメーション一枚のサイズ</param>
	/// <param name="maxAnimNum">最大アニメーション枚数</param>
	/// <param name="oneAnimFrame">次のアニメーションまでの待機時間</param>
	/// <param name="scale">表示サイズ</param>
	/// <param name="isRepeat">ループするかどうか</param>
	void Init(int drawHandle, int animIndex, Vector2 frameSize, int maxAnimNum, int oneAnimFrame, float scale, bool isRepeat);
	/// <summary>
	/// 普通のアニメーション初期化
	/// </summary>
	/// <param name="drawHandle">画像ハンドル</param>
	/// <param name="startSrcX">アニメーションを開始する切り取り位置</param>
	/// <param name="animIndexY">切り取り位置Y</param>
	/// <param name="frameSize">アニメーション一枚のサイズ</param>
	/// <param name="maxAnimNum">最大アニメーション枚数</param>
	/// <param name="oneAnimFrame">次のアニメーションまでの待機時間</param>
	/// <param name="scale">表示サイズ</param>
	/// <param name="isRepeat">ループするかどうか</param>
	void Init(int drawHandle, int startSrcX, int animIndexY, Vector2 frameSize, int maxAnimNum, int oneAnimFrame, float scale, bool isRepeat);
	/// <summary>
	/// 一枚絵アニメーション初期化
	/// </summary>
	/// <param name="drawHandle">画像ハンドル</param>
	/// <param name="animIndexY">縦切り取り位置</param>
	/// <param name="animIndexX">横切り取り位置</param>
	/// <param name="frameSize">画像1枚のサイズ</param>
	/// <param name="scale">表示サイズ</param>
	/// <param name="isRepeat">ループするかどうか</param>
	void Init(int drawHandle, int animIndexY, int animIndexX, Vector2 frameSize, float scale, bool isRepeat = true);
	void Update();
	void Draw(Vector2 pos, bool isTurn);
	void Draw(int drawHandle, Vector2 pos, bool isTurn);

	void SetRotate(float rotate) { m_rotate = rotate; }
	void SetOffset(Vector2 offset) { m_drawOffset = offset; }

	// アニメーションを最初から再生する
	void SetFirst() { m_nowAnimNum = 0; m_frameCount = 0; }
	// アニメーションを最後のコマにする
	void SetEnd() { m_nowAnimNum = m_maxAnimNum; }

	bool GetIsEnd();

	//現在のアニメーションフレーム数を取得
	const int GetNowAnimNum() const { return m_nowAnimNum; }

	bool operator!=(const Animation& other) const;
private:
	bool m_isRepeat;		// 繰り返し再生するかどうか
	int m_drawHandle;	// 画像ハンドル
	int m_animIndex;		// アニメーションの種類番号
	int m_startSrcX;		// アニメーション開始の切り取りX位置
	Vector2 m_frameSize;	// 画像の1コマサイズ
	int m_maxAnimNum;	// アニメーションの最大コマ数
	int m_nowAnimNum;	// 現在のアニメーション番号
	int m_oneAnimFrame;	// 1コマあたりのフレーム数
	int m_frameCount;	// アニメーション制御用
	float m_scale;		// 拡大率
	float m_rotate;		// 回転角
	Vector2 m_drawOffset; // 描画位置オフセット
};