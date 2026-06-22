#pragma once
#include "Utility/Geometry.h"

class BackGround
{
public:
	BackGround();
	~BackGround();

	void Update();
	void Draw();

private:
	int m_handle;
	Vector2 m_pos;//ˆÊ’u
};

