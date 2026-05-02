#pragma once
#include "Object.h"
#include <memory>
#include "../Collider/Circle.h"

struct GameContext;
class EffectFactory;
class Map;
class Bullet : public Object
{
public:
	Bullet(std::shared_ptr<EffectFactory> effectfactory, std::shared_ptr<Map> pMap);
	virtual ~Bullet();

	virtual void Init() override;
	virtual void Update(GameContext& ctx) override;
	virtual void Draw(std::shared_ptr<Camera> pCamera) override;

	const Vector2& GetPos() const { return m_pos; }
	void SetPos(const Vector2& pos) { m_pos = pos; }
	const bool GetIsAlive() const { return m_isAlive; }
	virtual void OnDead();
	void OnShot();

	const Circle& GetCircle() const { return m_circle; }

private:

protected:
	std::shared_ptr<Map> m_pMap;
	Vector2 m_dir;
	bool m_isAlive;
	Circle m_circle;

	std::shared_ptr<EffectFactory> m_pEffectFactory;
};

