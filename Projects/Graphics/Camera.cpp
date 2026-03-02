#include "Camera.h"
#include "../Objects/Player.h"
#include "../General/GameConstants.h"
#include <cmath>
#include "../Stages/Stage.h"
#include <DxLib.h>

namespace
{
	const Vector2 first_pos = { 0.0f,0.0f };
	constexpr int scorp_range = 400;
	constexpr float camera_lerp_t = 0.1f;
	constexpr int mapchip_size = 32;
	constexpr int shake_frame = 30;//カメラを揺らす時間
	//constexpr int shake_power = 7;//カメラを揺らすときの力
}

Vector2 Camera::VLerp(const Vector2& start, const Vector2& end, float t)
{
	Vector2 ret;
	ret.x = std::lerp(start.x, end.x, t);
	ret.y = std::lerp(start.y, end.y, t);
	return ret;
}

void Camera::OnImpact(int shakePower)
{
	//カメラを揺らす時間を設定
	m_shakingFrame = shake_frame;
	m_shakePower = shakePower;
}

void Camera::Impact()
{
	m_shakingFrame--;
	if (m_shakingFrame > 0)
	{
		//0からshake_powerの2倍を用意(右から左まで行くにはshake_powerの2倍必要だから)
		//そこからshake_powerを引くことでマイナス側も用意する
		//揺れる範囲は shake_power ～  -shake_power
		m_pos.y += GetRand(m_shakePower * 2 + 1) - m_shakePower;
		m_pos.x += GetRand(m_shakePower * 2 + 1) - m_shakePower;
	}
}

void Camera::OnArriveEnemy(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage)
{
	//画面の半分
	const int screenHalfW = Graphic::screen_width / 2;//640
	//カメラの位置がステージの端から画面サイズの半分を
	//引いた場所についたらカメラの位置を補正する
	const float posX = static_cast<float>(pStage->GetMapSize().w * mapchip_size - screenHalfW);
	const float posY = pPlayer->GetPos().y;
	Vector2 cameraEndPos = { posX,posY };
	//カメラの位置をボス部屋に合わせる
	m_pos = VLerp(m_pos, cameraEndPos, 0.07f);
	m_isArrive = true;
}

Camera::Camera() :
	m_pos(first_pos),
	m_isArrive(false),
	m_drawOffset(first_pos),
	m_shakingFrame(0),
	m_shakePower(0)
{

}

Camera::~Camera()
{

}

void Camera::Update(const std::shared_ptr<Player> pPlayer, const std::shared_ptr<Stage>& pStage)
{
	if (!m_isArrive)
	{
		//プレイヤーの位置とカメラの位置を同じにする
		m_pos = VLerp(m_pos, pPlayer->GetPos(), camera_lerp_t);
	}

	//ステージの端が見えたらそこからカメラを移動させない
	//ステージデータ取得
	const auto& mapSize = pStage->GetMapSize();
	//画面の半分
	const int screenHalfW = Graphic::screen_width / 2;//640
	//カメラの位置がステージの端から画面サイズの半分を
	//引いた場所についたらカメラの位置を補正する
	const int cameraEndPosX = pStage->GetMapSize().w * mapchip_size - screenHalfW;
	const int cameraStartPosX = screenHalfW;
	//カメラの終端位置と開始位置を指定する
	//その範囲内に収める
	if (m_pos.x > cameraEndPosX)
	{
		m_pos.x = static_cast<float>(cameraEndPosX);
	}
	else if (m_pos.x < cameraStartPosX)
	{
		m_pos.x = static_cast<float>(cameraStartPosX);
	}

	//カメラの揺れ処理
	Impact();

	//ベクトルや計算を使って、「カメラのポジションを動かす」
	//という感覚を保ちたいので、Draw側に足しているcamera.posをいじる
	m_drawOffset.x = m_pos.x * -1;
	//m_drawOffset.y = m_pos.y * -1;
	//その時画面の中央にプレイヤーが来るようにする
	//(camera.posが画面の中央になるようにする) 
	m_drawOffset.x += Graphic::screen_width / 2;
	//m_drawOffset.y += Graphic::screen_height / 2;
}