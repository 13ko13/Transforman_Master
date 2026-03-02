#pragma once
#include "Utility/Geometry.h"

/// <summary>
/// 対象の位置を提供するプロバイダー
/// </summary>
struct PosProvider
{
	//=defaultを付けると、中身を書かなくてもデフォルトの実装が使われる
	virtual ~PosProvider() = default;///仮想デストラクタ
	virtual Vector2 GetPos() const = 0;
};