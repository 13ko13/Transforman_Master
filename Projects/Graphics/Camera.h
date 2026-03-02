#pragma once
#include "../Utility/Geometry.h"
#include <memory>

class Player;
class Stage;
class GameManager;
class Camera
{
public:
	Camera();
	~Camera();

	void Update(const std::shared_ptr<Player> pPlayer,const std::shared_ptr<Stage>& pStage);

	const Vector2& GetDrawOffset() const { return m_drawOffset; };
	Vector2 VLerp(const Vector2& start, const Vector2& end, float t);
	/// <summary>
	/// カメラが衝撃を受けたときの関数
	/// </summary>
	void OnImpact(int shakePower);

	void OnArriveEnemy(std::shared_ptr<Player> pPlayer,std::shared_ptr<Stage> pStage);
	Vector2 GetPos() { return m_pos; };

private:
	//変数
	Vector2 m_pos;	//実際のカメラのポジション
	Vector2 m_drawOffset;	//全てのDrawObjectに足す値
	bool m_isArrive;//ボスの場所に到着したかどうか
	int m_shakingFrame;//現在のカメラを揺らしている時間
	int m_shakePower;//カメラを揺らす力 

	//関数
	/// <summary>
	/// カメラに衝撃を与える関数
	/// </summary>
	void Impact();
};

