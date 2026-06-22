#include "Bullet.h"
#include "../Graphics/Camera.h"
#include "../Collider/Circle.h"
#include "../EffectFactory.h"
#include "../Map.h"

Bullet::Bullet(std::shared_ptr<EffectFactory> effectfactory, std::shared_ptr<Map> pMap) :
	Object({ 0.0f,0.0f }, { 0.0f,0.0f }),
	m_isAlive(false),
	m_dir({ 0.0f,0.0f }),
	m_pEffectFactory(effectfactory)
{
	m_pMap = pMap;
}

Bullet::~Bullet()
{

}

void Bullet::Init()
{

}

void Bullet::Update(GameContext&)
{

}

void Bullet::Draw(std::shared_ptr<Camera> pCamera)
{

}

void Bullet::OnDead()
{
	
}

void Bullet::OnShot()
{
	m_isAlive = true;
}