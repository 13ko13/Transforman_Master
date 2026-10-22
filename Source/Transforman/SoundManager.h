#pragma once
#include <vector>
#include <string>
#include <map>
#include <cassert>

enum class SoundType
{
	Decision,//決定
	NormalShot,//通常ショット
	GameBgm1,//ゲームシーンBGM1
	GameBgm2,//ゲームシーンBGM2
	GameBgm3,//ゲームシーンBGM3
	TitleBgm,//タイトルシーンBGM
	ClearBgm,//クリアシーンBGM
	GameoverBgm,//ゲームオーバーシーンBGM
	BossShot,//ボスのショット
	PlayerCharge,//プレイヤーのチャージ
	PlayerChargeFinished,//プレイヤーのチャージ完了音
	PlayerChargeShot,//プレイヤーのチャージショット音
	Jump,//プレイヤーのジャンプ音
	HitShot,//弾が当たった時の音
	Pinch,//プレイヤーのHPがピンチになった時の音
	Rush,//敵の突進時の音
	Thunder,//雷の音
	PlayerDamage,//プレイヤーがダメージを食らった時の音
	PlayerDeath,//プレイヤーが死んだときの音
	Stan,//チャージショットボスの音
	EnemyDeath,//敵が死んだときの音
	ParrySuccess,//パリィが成功したときの音
	Parry,//パリィを発動したときの音
};

struct SoundData
{
	int handle = -1;//サウンドハンドル
	int volume = 255;//0～255
	bool isLoop = false;//デフォルトのループ設定
	bool loaded = false;//ロード済みフラグ
};

class SoundManager
{
public:
	/// <summary>
	/// SoundManagerのシングルトンインスタンスを取得する
	/// </summary>
	/// <returns>SoundManagerのインスタンス</returns>
	/// <note>SoundManagerの実体はこの関数内部で生成され、ずっととどまり続ける
	static SoundManager& GetInstance();

	~SoundManager();

	/// <summary>
	/// 音に関する情報をロードする関数
	/// </summary>
	/// <param name="type">音の種類</param>
	/// <param name="path">ファイル名</param>
	/// <param name="volume">音量</param>
	/// <param name="defaultLoop">デフォルトのループ設定</param>
	void Load(
		SoundType type,
		const std::string& path,
		int volume = 255,
		bool defaultLoop = false);

	/// <summary>
	/// 再生
	/// </summary>
	/// <param name="soundType">音の種類</param>
	void Play(SoundType soundType);

	/// <summary>
	/// 再生(ループするとき)
	/// </summary>
	/// <param name="soundType">種類</param>
	/// <param name="loop">ループするかどうか(指定しないとループしない)</param>
	void Play(SoundType soundType,bool loop);

	/// <summary>
	/// 指定の音を停止する
	/// </summary>
	/// <param name="type">音の種類</param>
	void StopSound(SoundType type);

	/// <summary>
	/// すべての音を停止させる
	/// </summary>
	void StopAll();

	/// <summary>
	/// 音量を変更
	/// </summary>
	/// <param name="type">音の種類</param>
	/// <param name="volume">音量</param>
	void SetVolume(SoundType type, int volume);

	/// <summary>
	/// ロードしているかどうかを返す関数
	/// </summary>
	/// <param name="type">種類</param>
	/// <returns>ロード済み:true,未ロード:false</returns>
	bool IsLoaded(SoundType type) const;

private:
	std::map<SoundType, SoundData> m_sounds;

private:
	SoundManager();//newも変数宣言もできないようにする
	SoundManager(const SoundManager& sm) = delete;//コピーコンストラクタを削除
	void operator=(const SoundManager& sm) = delete;//代入を削除
};

