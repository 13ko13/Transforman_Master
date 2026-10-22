#pragma once
#include "../Utility/Geometry.h"
#include "../SoundManager.h"

/// <summary>
/// アプリケーション全体を管理する
/// シングルトンクラス
/// </summary>
class Application
{ 
private:
	Size m_windowSize;

	Application();//newも変数宣言もできなくなる
	Application(const Application& app) = delete;//コピーコンストラクタを無効
	void operator=(const Application& app) = delete;//代入を禁止

	void SoundLoad(SoundManager& sm);

public:
	~Application();
	
	/// <summary>
	/// Applicationのシングルトンインスタンスを取得する
	/// </summary>
	/// <returns>Applicationのインスタンス</returns>
	/// <note>Applicationの実体はこの関数内部で生成され、ずっととどまり続ける</note>
	static Application& GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <returns>初期化成功:true / 初期化失敗:false</returns>
	bool Init();

	/// <summary>
	/// アプリケーションをスタートする
	/// ゲームループを内包する
	/// </summary>
	void Run();

	/// <summary>
	/// アプリケーション終了処理
	/// </summary>
	void Terminate();

	const Size& GetWindowSize() const;
};