#include "FlameBoss.h"
#include <Dxlib.h>
#include <cassert>

namespace
{
	constexpr int size_width = 200;
	constexpr int size_height = 200;
	//初期位置
	const Vector2 first_pos = { 3000.0f, -50.0f };
}

FlameBoss::FlameBoss(std::shared_ptr<Map> pMap, std::shared_ptr<EffectFactory> effectfactory) :
	EnemyBase(size_width, size_height, pMap, effectfactory)
{
	m_pos = first_pos;
	m_pos = first_pos;

	//画像ハンドルのロード
	m_handles.push_back(LoadGraph("img/game/Enemy/flame_breath.png"));
	assert(m_handles.back() >= 0);
	m_handles.push_back(LoadGraph("img/game/Enemy/flame_idle.png"));
	assert(m_handles.back() >= 0);
}

FlameBoss::~FlameBoss()
{
}

void FlameBoss::Init()
{
}

void FlameBoss::Update(GameContext& ctx)
{
}

void FlameBoss::Draw(std::shared_ptr<Camera> pCamera)
{
}

void FlameBoss::OnArrive()
{
}

void FlameBoss::OnDamage(bool isChargeShot)
{
}

void FlameBoss::OnParried(int dir)
{

}

void FlameBoss::Attack()
{
}
