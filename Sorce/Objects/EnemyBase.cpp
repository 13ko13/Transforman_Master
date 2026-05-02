#include "EnemyBase.h"
#include "../Graphics/Camera.h"

EnemyBase::EnemyBase(int width,int height,std::shared_ptr<Map> pMap,
	std::shared_ptr<EffectFactory> effectfactory):
	Charactor(width,height,pMap,effectfactory),
	m_appearTime(0),
	m_isAppear(false),
	m_hitPoint(0)
{
	
}		

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Init()
{

}

void EnemyBase::Update(GameContext& ctx)
{
	
}

void EnemyBase::Draw(std::shared_ptr<Camera> pCamera)
{

}

void EnemyBase::Attack()
{

}