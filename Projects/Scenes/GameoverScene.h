#pragma once
#include "Scene.h"
#include "SceneController.h"
#include "../Graphics/Animation.h"

class GameoverScene : public Scene
{
public:
	GameoverScene(SceneController&);
	~GameoverScene();
	void Init() override;
	void Update(Input&) override;
	void Draw() override;

private:
	enum class handleNumber
	{
		gameoverText,
		gameoverBackground,
		Abutton,//Press A Button
	};

	std::vector<int> m_handles;//ゲームオーバーのテキスト画像ハンドル
	int m_frame;//フェード用のフレーム

	Vector2 m_drawOffset;//描画だけずらす

	//メンバ変数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のGameoverSceneの状態に合わせて
	//切り替えていく
	//注意:関数名は動詞が先になるように書く
	void UpdateFadeIn(Input&);//フェードインUpdate
	void UpdateNormal(Input&);//通常状態Update
	void UpdateFadeOut(Input&);//フェードアウトUpdate

	//関数ポインタを使って状態遷移を楽にする
	using UpdateFunc_t = void(GameoverScene::*)(Input&);
	UpdateFunc_t m_update;//updateメンバ関数ポインタ

	void DrawFade();//フェードインDraw
	void DrawNormal();//通常状態Draw

	//関数ポインタを使って状態遷移を楽にする
	using DrawFunc_t = void(GameoverScene::*)();
	DrawFunc_t m_draw;//drawメンバ関数ポインタ

	Animation m_backgroundAnim;//ゲームオーバー背景アニメーション
};