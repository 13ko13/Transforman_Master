#define NOMINMAX
#include "TextScene.h"
#include "../General/Input.h"
#include "../Scenes/SceneController.h"
#include "../Main/Application.h"
#include <DxLib.h>

//コンストラクタ:画像をロード(貼るだけ用途)
TextScene::TextScene(SceneController& controller,
	const std::vector<PageDesc>& pages) :
	Scene(controller)
{
	//事前に必要なページ枚数分の配列を確保
	m_pages.reserve(pages.size());
	for (const auto& page : pages)
	{
		PageRuntime rt;
		rt.imageHandle = LoadGraph(page.imagePath.c_str());
		if (rt.imageHandle != 1)
		{
			GetGraphSize(rt.imageHandle, &rt.imgW, &rt.imgH);//画像サイズを獲得
		}
		//確保した画像のデータを配列に入れる
		m_pages.push_back(rt);
	}

	//ページが空なら即閉じる
	if (m_pages.empty())
	{
		controller.PopScene();
	}
}

TextScene::~TextScene()
{
	//ロードした画像の破棄
	for (auto& page : m_pages)
	{
		if (page.imageHandle != -1)
		{
			DeleteGraph(page.imageHandle);
			page.imageHandle = -1;
		}
	}
}

void TextScene::Init()
{

}

//入力 : R1 = 次,L1 = 前,最後ページでB = 戻る
void TextScene::Update(Input& input)
{
	if (m_pages.empty()) return;//表示するページがないなら飛ばす

	if (input.IsTriggered("next"))
	{
		//次へ
		NextPage();
		return;
	}
	if (input.IsTriggered("next"))
	{
		//前へ
		PrevPage();
		return;
	}
	if (input.IsTriggered("close"))
	{
		//最終ページなら閉じる
		if (m_currentIndex == static_cast<int>(m_pages.size()) - 1)
		{
			m_controller.PopScene();
			return;
		}
	}
}

//描画 : 画像を中央に貼るだけ
void TextScene::Draw()
{
	if (m_pages.empty()) return;
	DrawCurrentImage();
}

//現在ページの画像を中央に表示
//画像側に外枠等が含まれている前提なので、シーン側の飾りは一切しない。
//画像が大きすぎる場合のみ、最大枠にフィットさせて比率維持で縮小
void TextScene::DrawCurrentImage()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//表示最大枠
	constexpr int maxImgW = 640;
	constexpr int maxImgH = 360;

	const auto& page = m_pages[m_currentIndex];
	if (page.imageHandle == -1) return;

	DrawRotaGraph(200, 100, 1.0, 0.0, page.imageHandle, true);
}

//ページ遷移(循環しない設計)
void TextScene::NextPage()
{
	if (m_currentIndex < static_cast<int>(m_pages.size()) - 1)
	{
		m_currentIndex++;
	}
}

void TextScene::PrevPage()
{
	if (m_currentIndex > 0)
	{
		m_currentIndex--;
	}
}