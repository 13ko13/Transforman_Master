#include "SoundManager.h"
#include <DxLib.h>
#include <algorithm>

bool SoundManager::IsLoaded(SoundType type) const
{
	auto it = m_sounds.find(type);
	return (it != m_sounds.end() && it->second.loaded);
}

SoundManager::SoundManager()
{
	//読み込んだ音をすべて開放する
	for (auto& [type, data] : m_sounds)
	{
		if (data.loaded && data.handle != -1)
		{
			DeleteSoundMem(data.handle);
		}
	}
}

SoundManager& SoundManager::GetInstance()
{
	//staticで宣言してそれを返す
	static SoundManager instance;
	return instance;
}

SoundManager::~SoundManager()
{
}

void SoundManager::Load(SoundType type, const std::string& path, int volume, bool defaultLoop)
{
	auto& data = m_sounds[type];//音の情報を取得

	//既にロードしていたらロードしない
	if (data.loaded) return;

	int handle = LoadSoundMem(path.c_str());//パスを通してサウンドハンドルを取得
	assert(handle != - 1&& "音をロードできていません");//ロードできていなければクラッシュ

	data.handle = handle;
	data.loaded = true;
	data.isLoop = defaultLoop;

	//音量設定(0～255)
	data.volume = std::clamp(volume, 0, 255);//0以下255を越えていたら範囲内に収める
	ChangeVolumeSoundMem(data.volume, data.handle);
}

void SoundManager::Play(SoundType soundType)
{
	//ロードされていなかったら以降の処理をスキップ
	auto it = m_sounds.find(soundType);
	if (it == m_sounds.end() || !it->second.loaded) return;

	const auto& data = it->second;//今回指定された種類の音の情報を保存
	//ループ指定ならループさせる
	const bool isLoop = data.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	//音を再生する
	PlaySoundMem(
		data.handle,
		isLoop,
		true);
}

void SoundManager::Play(SoundType soundType, bool loop)
{
	//ロードされていなかったら以降の処理をスキップ
	auto it = m_sounds.find(soundType);
	if (it == m_sounds.end() || !it->second.loaded) return;

	const auto& data = it->second;
	//音を再生する
	PlaySoundMem(
		data.handle,
		loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK,//loopがtrueならループそうじゃないならバック再生
		true);
}

void SoundManager::StopSound(SoundType type)
{
	auto it = m_sounds.find(type);
	if (it == m_sounds.end() || !it->second.loaded) return;

	//指定した音を停止させる
	StopSoundMem(it->second.handle);
}

void SoundManager::StopAll()
{
	for (auto sound : m_sounds)
	{
		//すべての音を停止する
		StopSoundMem(sound.second.handle);
	}
}

void SoundManager::SetVolume(SoundType type, int volume)
{
	auto it = m_sounds.find(type);
	if (it == m_sounds.end()) return;

	auto& data = it->second;
	//音量を矯正
	data.volume = std::clamp(volume, 0, 255);

	//ちゃんとロードされてたら音量を変更する
	if (data.loaded && data.handle != -1)
	{
		ChangeVolumeSoundMem(data.volume, data.handle);
	}
}
