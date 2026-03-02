#pragma once
#include "Utility/Geometry.h"

class UserInterface
{
public:
	UserInterface(Vector2 pos);
	virtual ~UserInterface();

	virtual void Update() abstract;
	virtual void Draw() abstract;

private:

protected:
	Vector2 m_pos;//ˆÊ’u
};

