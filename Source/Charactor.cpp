#include "Charactor.h"
#include "Graphics/Camera.h"
#include "EffectFactory.h"
#include "Map.h"
#include <DxLib.h>

namespace
{
	constexpr int hit_action_time = 10;//ダメージを受けたときの赤くなる 時間
}

Charactor::Charactor(int width, int height, std::shared_ptr<Map> pMap,
	std::shared_ptr<EffectFactory> effectfactory) :
	Object({ 0.0f,0.0f }, { 0.0f,0.0f }),
	m_hitActionTimer(0),
	m_isGround(false),
	m_isRight(false),
	m_isDamage(false),
	m_isDead(false),
	m_width(width),
	m_height(height)
{
	m_pMap = pMap;
	m_pEffectFactory = effectfactory;
	//当たり判定を初期化
	m_colRect.SetCenter(m_pos.x, m_pos.y, static_cast<float>(m_width - 1), static_cast<float>(m_height - 1));
}

Charactor::~Charactor()
{	

}

void Charactor::Init()
{
}

void Charactor::Update(GameContext& ctx)
{
	//被ダメージ時の赤くなる時間を計測
	m_hitActionTimer--;
	if (m_hitActionTimer <= 0)
	{
		m_isDamage = false;
		m_hitActionTimer = 0;
	}

	m_isGround = false;
	Gravity();

	if (m_isGround)
	{
		m_velocity.y = 0.0f;
	}
}

void Charactor::Draw(std::shared_ptr<Camera> pCamera)
{
#ifdef _DEBUG
	m_colRect.Draw(0xff0000, false, pCamera);	
#endif // DEBUG
}

void Charactor::HitMap(Rect& chipRect)
{
	//横方向の衝突
	m_pos.x += m_velocity.x;
	//常に最新の矩形情報にする
	m_colRect.SetCenter(m_pos.x, m_pos.y, static_cast<float>(m_width - 1), static_cast<float>(m_height - 1));
	//衝突していたら
	if (m_pMap->IsCollision(m_colRect, chipRect))
	{
		//キャラが右に移動しているなら
		if (m_velocity.x > 0.0f)
		{
			//衝突したら当たったチップの左を取得して
			//そこからキャラの幅の半分だけ補正する
			m_pos.x = chipRect.GetLeft() - m_width * 0.5f;
		}
		else if (m_velocity.x < 0.0f)//キャラが左に移動しているなら
		{
			//衝突したら当たったチップの右を取得して
			//そこからキャラの幅の半分だけ補正する
			m_pos.x = chipRect.GetRight() + m_width * 0.5f;
		}
		//最後に速度を0にする
		m_velocity.x = 0.0f;
	}

	//斜めから当たったりしたときの優先順位をつけるために
	//軸を分離して判定する
	//縦方向の衝突
	m_pos.y += m_velocity.y;
	//常に最新の矩形情報にする
	m_colRect.SetCenter(m_pos.x, m_pos.y, static_cast<float>(m_width - 1), static_cast<float>(m_height - 1));
	if (m_pMap->IsCollision(m_colRect, chipRect))
	{
		//下方向(落下)
		if (m_velocity.y > 0.0f)
		{
			//衝突したら、当たったチップの上を取得してそこから、
			//キャラの高さの半分だけ補正する
			m_pos.y = chipRect.GetTop() - m_height * 0.5f;
			m_velocity.y = 0.0f;//衝突したのでvelocity.yを0にする
			m_isGround = true;
		}
		else if (m_velocity.y < 0.0f)//上方向(ジャンプ中)
		{
			//衝突したら、当たったチップの下を取得してそこから、
			//キャラの高さの半分だけ補正
			m_pos.y = chipRect.GetBottom() + m_height * 0.5f;
			//ビタッと止まらないように少しずつvelocityを減少させる
			m_velocity.y *= -0.1f;
		}
	}

#ifdef _DEBUG
	m_colRect.SetCenter(m_pos.x, m_pos.y, static_cast<float>(m_width), static_cast<float>(m_height));
#endif // DEBUG
}

void Charactor::OnDamage()
{
	//キャラクター全員に共通したダメージ処理
	m_hitActionTimer = hit_action_time;
	m_isDamage = true;
}