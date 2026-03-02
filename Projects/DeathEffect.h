#pragma once
#include "Effect.h"
#include "Graphics/Animation.h"

enum class DeathCharactor
{
	Player,
	Enemy
};

class DeathEffect : public Effect
{
public:
	DeathEffect(int handle, const Vector2& pos,DeathCharactor charactor);

	void Update() override;
	void Draw(const std::shared_ptr<Camera>& pCamera) override;

private:
	int m_handle;
	Animation m_playerAnimation;
	Animation m_enemyAnimation;

	DeathCharactor m_deathCharactor;
};