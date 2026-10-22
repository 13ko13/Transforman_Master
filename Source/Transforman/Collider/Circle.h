#pragma once
#include "../Utility/Geometry.h"
#include <memory>

class Rect;
class Camera;
class Circle
{
public:
	Circle();
	~Circle();

	void Draw(std::shared_ptr<Camera> pCamera);
	void SetPos(const Vector2& pos);
	/// <summary>
	/// ’†SŠÔ‹——£‚ğ‹‚ß‚é
	/// </summary>
	/// <returns>’†S‹——£</returns>
	const float GetDistance(Vector2& pos) const;

	/// <summary>
	/// ‰~‚Æ‹éŒ`‚Ì“–‚½‚è”»’è
	/// </summary>
	/// <returns>‹éŒ`‚Æ“–‚½‚Á‚Ä‚¢‚é:true,“–‚½‚Á‚Ä‚¢‚È‚¢:false</returns>
	const bool IsCollWithRect(const Rect& rect) const;

	/// <summary>
	/// •`‰æ‚·‚éÛ‚É”¼Œa‚ğŒˆ‚ß‚é
	/// </summary>
	/// <param name="radius">”¼Œa</param>
	void SetRadius(int radius) { m_radius = radius; }

	Vector2 GetCenter() const { return m_pos; }
private:
	Vector2 m_pos;
	int m_radius;
};

