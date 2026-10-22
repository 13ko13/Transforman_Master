#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include "../StageType.h"
#include "../Utility/Geometry.h"

//プロトタイプ宣言
class Input;
class Player;
class ChargeShotBoss;
class EnemyBullet;
class Camera;
class EnemyBase;
class CollisionManager;
class Object;
class Stage;
class BackGround;
class Map;
class Charactor;
class GameManager;
class Text;
class ParryBoss;
class UIManager;
class EffectFactory;
class GameScene : public Scene
{
public:
	GameScene(SceneController& controller,StageType stageType);
	~GameScene();
	void Init() override;
	void Update(Input& input) override;
	void Draw()override;
private:
	enum class HandleNomber
	{
		Jump,//ジャンプ
		Move,//移動
		Shot,//弾
		Parry,//パリィ
	};

	std::vector<int> m_handles;//画像ハンドル

	std::shared_ptr<Player> m_pPlayer;//プレイヤー
	std::vector<std::shared_ptr<PlayerBullet>> m_pPlayerBullets;//プレイヤー弾
	std::vector<std::shared_ptr<EnemyBullet>> m_pEnemyBullets;//敵の弾
	std::vector<std::shared_ptr<EnemyBase>> m_pEnemies;//敵
	std::shared_ptr<ChargeShotBoss> m_pChargeShotBoss;//チャージショットボス
	std::shared_ptr<ParryBoss> m_pParryBoss;//パリィボス
	std::shared_ptr<Camera> m_pCamera;//カメラ
	std::shared_ptr<CollisionManager> m_pColManager;//当たり判定のマネージャー
	std::vector<std::shared_ptr<Object>> m_pObjects;//オブジェクトの配列
	std::shared_ptr<Stage> m_pStage;//ステージデータ
	std::shared_ptr<BackGround> m_pBackground;//背景
	std::shared_ptr<Map> m_pMap;//マップチップ
	std::vector<std::shared_ptr<Charactor>> m_pCharactors;//キャラクターの配列
	std::shared_ptr<GameManager> m_pGameManager;//ゲームマネージャー
	std::shared_ptr<UIManager> m_pUIManager;//UIマネージャー
	std::shared_ptr<EffectFactory> m_pEffectFactory;//エフェクトファクトリー

	int m_frame;//フェード用のフレーム
	int m_fontHandle;//フォントのハンドル

	//関数ポインタをつかって状態遷移を楽にする
	using UpdateFunc_t = void(GameScene::*)(Input&);
	UpdateFunc_t m_update;///updateメンバ関数ポインタ

	//関数ポインタをつかって状態遷移を楽にする
	using DrawFunc_t = void(GameScene::*)();
	DrawFunc_t m_draw;//drawメンバ関数ポインタ

	bool m_isStageClear;
	bool m_isGameover;
	bool m_isGameClear;

	bool m_isFade;

	Vector2 m_stageTypeTextPos;
	int m_textFadeFrame;//テキストのフェード用フレーム

private:
	//メンバ変数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のGameSceneの状態に合わせて
	//切り替えていく

	/// <summary>
	/// フェードイン中のアップデート処理
	/// </summary>
	void UpdateFadeIn(Input&);//フェードインUpdate

	/// <summary>
	/// 通常更新中のアップデート処理
	/// </summary>
	void UpdateNormal(Input&);//通常状態Update

	/// <summary>
	/// フェードアウト中のアップデート処理
	/// </summary>
	void UpdateFadeOut(Input&);//フェードアウトUpdate

	/// <summary>
	/// フェード中の描画処理
	/// </summary>
	void DrawFade();//フェードインDraw

	/// <summary>
	/// 通常の描画操作を実行します。
	/// </summary>
	void DrawNormal();//通常状態Draw
};