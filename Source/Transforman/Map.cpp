#include "Map.h"
#include "Stages/Stage.h"
#include <DxLib.h>
#include <cassert>
#include "Main/Application.h"
#include "Graphics/Camera.h"
#include "Objects/Player.h"
#include <algorithm>


namespace
{
	constexpr int chip_size = 32;
	constexpr int graph_chip_row = 18;
}

Map::Map(std::shared_ptr<Stage> pStage) :
	m_pStage(pStage),
	m_isStageEnd(false),
	m_scrollX(0),
	m_startChipX(0),
	m_offsetX(0)
{
	//ステージ1のマップチップの読み込み
	int handle = -1;
	handle = LoadGraph("img/game/map/mapchip_1.png");
	m_handles.push_back(handle);
	//チェック
	assert(handle > -1);

	//ステージ2のマップチップの読み込み
	handle = LoadGraph("img/game/map/mapchip_2.png");
	m_handles.push_back(handle);
	assert(handle > -1);

	m_stageType = StageType::Stage1;
}

Map::~Map()
{
	//画像の開放
	for (auto handle : m_handles)
	{
		DeleteGraph(handle);
	}
}

void Map::Update()
{
}

void Map::Draw(Camera camera)
{
	//ステージデータとマップのサイズを取得
	const auto& stageData = m_pStage->GetAllData();
	const auto& mapSize = m_pStage->GetMapSize();
	//ウィンドウサイズを取得
	const auto& wsize = Application::GetInstance().GetWindowSize();
	for (int y = 0; y < mapSize.h; y++)
	{
		//マップの列をインデックスとして保管
		//スクロール対応するためにstartYを足してあげる
		//マップ外にアクセスしないようにcontinueする
		if (y < 0 || y >= mapSize.h) continue;

		//const int drawCols = std::min(chipOnScreenW + 1, mapSize.w - startX);
		for (int x = 0; x < mapSize.w; x++)
		{
			//スクロール対応するためにstartXを足してあげる
			//const int idxX =  startX + x;
			//マップ外にアクセスしないようにcontinueする
			if (x < 0 || x >= mapSize.w) continue;

			//現在のチップのIDをstageDataをもとに計算する
			auto chipID = stageData[x + y * mapSize.w];
			//チップのIDが0の時は透明なのでチップを配置しない
			if (chipID == 0) continue;

			const float posX = static_cast<float>(x * chip_size) +
				camera.GetDrawOffset().x;
			const float posY = static_cast<float>(y * chip_size) +
				camera.GetDrawOffset().y;
			//どのマップチップを表示するかを計算
			//インデックスを考慮するので余りを出して1行目以外にも対応
			//できるようにする(どこの行にいっても何列目というのがわかる)
			//最後に、これは切り取り位置なのでサイズをかけてあげる
			int srcX = (chipID % graph_chip_row) * chip_size;
			//何行目かがわかればいいので元画像のチップの
			//横に並んでいる数で割ると現在の列がわかる
			//最後に、これは切り取り位置なのでサイズをかけてあげる
			int srcY = (chipID / graph_chip_row) * chip_size;
			
			switch (m_stageType)
			{
			case StageType::Stage1:
			//描画
			DrawRectRotaGraph(
				static_cast<int>(posX + chip_size / 2),
				static_cast<int>(posY + chip_size / 2),
							srcX, srcY,
							chip_size, chip_size,
							1.0, 0.0,
							m_handles[static_cast<int>(StageType::Stage1)], true);//ステージ1のマップチップを描画
				break;
			case StageType::Stage2:
				//描画
				DrawRectRotaGraph(
					static_cast<int>(posX + chip_size / 2),
					static_cast<int>(posY + chip_size / 2),
					srcX, srcY,
					chip_size, chip_size,
					1.0, 0.0,
					m_handles[static_cast<int>(StageType::Stage2)], true);//ステージ2のマップチップを描画
				break;
			}
			

#ifdef _DEBUG
			//当たり判定
			DrawBoxAA(posX, posY, posX + chip_size, posY + chip_size, 0x00ff00, false);
#endif // _DEBUG

		}
	}
}

bool Map::IsCollision(const Rect hitRect, Rect& chipRect)
{
	//ステージデータとマップのサイズを取得
	const auto& stageData = m_pStage->GetAllData();
	const auto& mapSize = m_pStage->GetMapSize();
	//ウィンドウサイズを取得
	const auto& wsize = Application::GetInstance().GetWindowSize();
	for (int y = 0; y < mapSize.h; y++)
	{
		//マップの高さを越えたらcontinueする
		if (y >= mapSize.h) continue;
		for (int x = 0; x < mapSize.w; x++)
		{
			//マップの幅を越えたらcontinueする	
			if (x >= mapSize.w) continue;
			//現在のチップのIDをstageDataをもとに計算する
			auto chipID = stageData[x + y * mapSize.w];
			//0番は透明なのでcontinueする
			if (chipID == 0) continue;
			int chipLeft = static_cast<int>(x * chip_size);
			int chipRight = static_cast<int>(chipLeft + chip_size);
			int chipTop = static_cast<int>(y * chip_size);
			int chipBottom = static_cast<int>(chipTop + chip_size);

			//絶対に当たらない場合
			if (chipLeft > hitRect.GetRight()) continue;
			if (chipTop > hitRect.GetBottom()) continue;
			if (chipRight < hitRect.GetLeft()) continue;
			if (chipBottom < hitRect.GetTop()) continue;

			//ぶつかったマップチップの矩形を設定する
			chipRect.m_left = static_cast<float>(chipLeft);
			chipRect.m_right = static_cast<float>(chipRight);
			chipRect.m_top = static_cast<float>(chipTop);
			chipRect.m_bottom = static_cast<float>(chipBottom);

			//いずれかのチップに当たっていたら終了する
			return true;
		}
	}
	//どれにも当たっていない
	return false;
}
