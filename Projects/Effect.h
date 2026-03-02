#pragma once
#include "Utility/Geometry.h"
#include <memory>

struct PosProvider;
class Camera;
class Effect
{
public:
    Effect(const Vector2& pos);
	virtual ~Effect();
    virtual void Update();
    virtual void Draw(const std::shared_ptr<Camera>& pCamera);
	bool GetIsDead() const { return m_isDead; }
    const Vector2 GetPos() const { return m_pos; }
	void Kill() { m_isDead = true; }//外部からエフェクトを消すための関数

protected:
	Vector2 m_pos;
    bool m_isDead;

    //エフェクト追従用
    std::weak_ptr<PosProvider> m_target;//位置提供者(寿命管理のためweak_ptr)
	Vector2 m_offset;//オフセット
	bool m_isFollow = false;//追従エフェクトかどうか
};

