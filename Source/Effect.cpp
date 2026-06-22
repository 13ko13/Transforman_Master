#include "Effect.h"

Effect::Effect(const Vector2& pos)
{
    m_isDead = false;
	m_offset = { 0.0f,0.0f };
    m_pos = pos;
}

Effect::~Effect()
{
}

void Effect::Update()
{
    
}

void Effect::Draw(const std::shared_ptr<Camera>& pCamera)
{
}


