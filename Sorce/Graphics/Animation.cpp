#include "Animation.h"
#include "Dxlib.h"

Animation::Animation() :
	m_isRepeat(true),
	m_drawHandle(-1),
	m_animIndex(0),
	m_frameSize({ 0.0f,0.0f }),
	m_maxAnimNum(0),
	m_nowAnimNum(0),
	m_oneAnimFrame(0),
	m_frameCount(0),
	m_scale(1.0f),
	m_rotate(0.0f),
	m_drawOffset({0.0f,0.0f}),
	m_startSrcX(0)
{
}

Animation::~Animation()
{
}


void Animation::Init(int drawHandle, int startSrcX, int animIndexY, Vector2 frameSize,
	int maxAnimNum, int oneAnimFrame, float scale, bool isRepeat)
{
	m_isRepeat = isRepeat;
	m_drawHandle = drawHandle;
	m_animIndex = animIndexY;
	m_startSrcX = startSrcX;       // ← 開始列（シート左からのXコマ番号）
	m_frameSize = frameSize;
	m_maxAnimNum = maxAnimNum;      // ← 「枚数」のみを保持（startSrcXは足さない）
	m_oneAnimFrame = oneAnimFrame;
	m_scale = scale;
	m_nowAnimNum = 0;               // ← 0..(m_maxAnimNum-1) の「相対インデックス」で進める
	m_frameCount = 0;
}



void Animation::Init(int drawHandle, int animIndex, Vector2 frameSize,
	int maxAnimNum, int oneAnimFrame, float scale, bool isRepeat)
{
	m_isRepeat = isRepeat;
	m_drawHandle = drawHandle;
	m_animIndex = animIndex;
	m_startSrcX = 0;               // ← 先頭から始める
	m_frameSize = frameSize;
	m_maxAnimNum = maxAnimNum;      // ← 枚数
	m_oneAnimFrame = oneAnimFrame;
	m_scale = scale;
	m_nowAnimNum = 0;
	m_frameCount = 0;
}


void Animation::Init(int drawHandle, int animIndexY, int animIndexX, Vector2 frameSize, float scale, bool isRepeat)
{
	m_isRepeat = isRepeat;
	m_drawHandle = drawHandle;
	m_animIndex = animIndexY;
	m_frameSize = frameSize;
	m_maxAnimNum = animIndexX + 1;
	m_oneAnimFrame = 0;
	m_scale = scale;
	m_nowAnimNum = animIndexX;
	m_frameCount = 0;
}


void Animation::Update()
{
	if (m_oneAnimFrame == 0) return;

	m_frameCount++;
	if (m_frameCount >= m_oneAnimFrame)
	{
		m_frameCount = 0;
		m_nowAnimNum++;
		if (m_nowAnimNum >= m_maxAnimNum)
		{
			if (m_isRepeat)
			{
				m_nowAnimNum = 0;                    // ループ
			}
			else
			{
				m_nowAnimNum = m_maxAnimNum - 1;     // ← 非ループは「最後のコマで止める」
			}
		}
	}
}


void Animation::Draw(Vector2 pos, bool isTurn)
{
	// ← 「最大なら描かない」条件を撤廃。常に現在コマを描く
	const int srcX = (m_startSrcX + m_nowAnimNum) * static_cast<int>(m_frameSize.x);
	const int srcY = m_animIndex * static_cast<int>(m_frameSize.y);

	DrawRectRotaGraph(static_cast<int>(pos.x + m_drawOffset.x), static_cast<int>(pos.y + m_drawOffset.y),
		srcX, srcY,
		static_cast<int>(m_frameSize.x), static_cast<int>(m_frameSize.y),
		m_scale, m_rotate, m_drawHandle, true, isTurn);
}

void Animation::Draw(int drawHandle, Vector2 pos, bool isTurn)
{
	const int srcX = (m_startSrcX + m_nowAnimNum) * static_cast<int>(m_frameSize.x);
	const int srcY = m_animIndex * static_cast<int>(m_frameSize.y);

	DrawRectRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y),
		srcX, srcY,
		static_cast<int>(m_frameSize.x), static_cast<int>(m_frameSize.y),
		m_scale, m_rotate, drawHandle, true, isTurn);
}


bool Animation::GetIsEnd()
{
	if (m_isRepeat) return false;

	// 「最後のコマの最後のフレーム」まで進んだら終了
	return (m_nowAnimNum == m_maxAnimNum - 1) && (m_frameCount == m_oneAnimFrame - 1);
}


bool Animation::operator!=(const Animation& other) const
{
	if (m_drawHandle != other.m_drawHandle) return true;
	if (m_animIndex != other.m_animIndex) return true;
	if (m_rotate != other.m_rotate) return true;
	if (m_oneAnimFrame != other.m_oneAnimFrame) return true;
	return false;
}
