#pragma once
#include "Bullet.h"
#include "../General/Input.h"
#include <memory>
#include "../Utility/Geometry.h"
#include "../Collider/Rect.h"
#include "../Graphics/Animation.h"

enum class BulletType
{
	Normal,
	Charge,
	Fire
};

class EnemyBase;
class PlayerBullet : public Bullet
{
public:
	PlayerBullet(std::shared_ptr<EffectFactory> pEffectFactory,std::shared_ptr<Map> pMap);
	~PlayerBullet();

	void Init() override;
	void Update(GameContext& ctx) override;
	void Draw(std::shared_ptr<Camera> pCamera) override;

	void SetIsRight(bool isRight) { m_isRight = isRight; }

	void SetType(BulletType bulletType) { m_bulletType = bulletType; }
	BulletType GetType() const { return m_bulletType; }

	/// <summary>
	/// 火炎放射を打つという命令が飛ばされたときに呼ぶ関数
	/// </summary>
	void OnFlame();

	/// <summary>
	/// 弾が消えるときの処理
	/// </summary>
	void OnDead();
	
private:
	int m_animFrame;//アニメーション中の時間
	bool m_isRight;
	BulletType m_bulletType;
	Rect m_rect;

	//炎が消えるまでの秒数
	float m_flameLifeTime;
	
	//アニメーション
	Animation m_normalAnim;
	Animation m_chargeAnim;

	std::vector<int> m_handles;//弾の画像ハンドル

	enum class HandleNumber
	{
		Normal = 0,
		Charge = 1
	};

private:
};