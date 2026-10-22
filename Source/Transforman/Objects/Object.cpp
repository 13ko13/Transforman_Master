#include "Object.h"
#include <DxLib.h>

namespace
{
	constexpr float gravity = 1.0f;
}

Object::Object(const Vector2& pos,const Vector2& vel) :
	m_pos(pos),
	m_velocity(vel),
	m_frame(0),
	m_handle(0),
	m_animFrame(0)
{

}

Object::~Object()
{
	
}

void Object::Update(GameContext& ctx)
{

}

void Object::Gravity()
{
	m_velocity.y += gravity;
}