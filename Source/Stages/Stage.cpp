#include "Stage.h"
#include <algorithm>
#include <DxLib.h>
#include <cassert>


namespace
{
	//【FMFファイルヘッダ(20 byte)】
	//	+ 0	identifier[U4]	// ファイル識別子 'FMF_' (0x5F464D46)
	//	+ 4	size[U4]	// ヘッダを除いたデータサイズ
	//	+ 8	mapWidth[U4]	// マップの横幅
	//	+ 12	mapHeight[U4]	// マップの縦幅
	//	+ 16	chipHeight[U1]	// パーツの横幅
	//	+ 17	chipHeight[U1]	// パーツの縦幅
	//	+ 18	layerCount[U1]	// レイヤー数
	//	+ 19	bitCount[U1]	// レイヤーデータのビットカウント(8/16)
	//配置に必要な情報
	struct DataSetting
	{
		//チップ幅(いらない)
		uint8_t chipW;
		//チップ高さ(いらない)
		uint8_t chipH;
		//レイヤー数
		uint8_t layerCount;
		//1チップに何ビット使用しているのか
		uint8_t bitCount;
	};

	//マップデータのヘッダ
	struct DataHeader
	{
		char identifer[4];//正しければ'FMF_'になっている
		uint32_t size;//データのサイズ
		uint32_t wisth;//データの幅
		uint32_t height;//データの高さ
		DataSetting setting;//データ配置に必要な情報
	};
}

void Stage::Load(int stageNom)
{
	//ファイルのパスを保管する変数
	char filePath[32];
	//stageNomに応じて、data/stageX.fmfというファイル名を作成
	sprintf_s(filePath, "data/stage%d.fmf", stageNom);
	//ファイルのパスを読み込んでそのファイルを
	// 開いてそのハンドルを保管
	auto handle = FileRead_open(filePath);
	//handle確認
	assert(handle > -1);//ハンドルが読み込まれていないとクラッシュする
	//マップデータのヘッダを作成
	DataHeader header;
	//ファイルの読み込み
	// (ファイルのハンドルを読み込んで
	// データヘッダをゲットする)
	FileRead_read(&header, sizeof(header), handle);
	//データの幅と高さを保管する
	m_dataSize.w = header.wisth;
	m_dataSize.h = header.height;
	//m_dataのサイズをヘッダのサイズを読み込んでresizeする
	m_data.resize(header.size);
	//まず、データ本体を一気に読み込む
	//(ヘッダはもう読み込んだのでFMFの残り部分(タイル情報)を
	//データの内部領域に格納、それをヘッダのサイズ分読み込む
	FileRead_read(m_data.data(), static_cast<int>(m_data.size() * sizeof(uint8_t)), handle);
	//読み込みは終了したのでファイルを閉じる(ハンドル)
	FileRead_close(handle);
}

Size Stage::GetMapSize() const
{
	return m_dataSize;
}

uint8_t Stage::GetData(int xidx, int yidx)
{
	return m_data[yidx * m_dataSize.w + xidx];
}

const std::vector<uint8_t>& Stage::GetAllData() const
{
	return m_data;
}