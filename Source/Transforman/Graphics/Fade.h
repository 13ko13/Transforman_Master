#pragma once

class GameScene;
class Fade
{
public:
	Fade();
	~Fade();

	void Update();
	void Draw();

	void FadeInUpdate();
	void FadeOutUpdate();
	using UpdateFunc_t = void(GameScene::*)();

	void FadeDraw();

	using DrawFunc_t = void(GameScene::*)();

private:
	int m_frame;///フェードインアウト用
};