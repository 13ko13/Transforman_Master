#pragma once
#include "Scene.h"
#include <string>

class TextScene :  public Scene
{
public:
	//1ページ分のデータ(画像パス)
	struct PageDesc
	{
		std::string imagePath;//ファイルパス
	};

	TextScene(SceneController& controller,const std::vector<PageDesc>& pages);
	~TextScene();//ロードした画像の破棄
	void Init() override;
	void Update(Input& input) override;
	void Draw()override;

private:
	//ロード済みページ(画像ハンドルとサイズ)
	struct PageRuntime
	{
		int imageHandle = -1;//LoadGraphのハンドル
		int imgW = 0;//画像幅
		int imgH = 0;//画像高さ
	};

	//画像を中央に表示(必要なら枠内にフィット)
	void DrawCurrentImage();

	//ページ遷移
	void NextPage();//R1
	void PrevPage();//L1

private:
	std::vector<PageRuntime> m_pages;//ロード済みページ配列
	int m_currentIndex = 0;//現在ページ番号
};