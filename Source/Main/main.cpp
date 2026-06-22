#include "DxLib.h"
#include "Application.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto& app = Application::GetInstance();
	if (!app.Init())
	{
		return -1;
	}
	app.Run();//ゲームスタート(ゲームループに入る)
	app.Terminate();//ゲーム終了処理
	return 0;				// ソフトの終了 
}