#pragma once
#include <map>
#include <vector>
#include <string>

/// <summary>
/// 周辺機器種別
/// </summary>
enum class PeripheralType
{
	keyboard,
	pad1
};

/// <summary>
/// 入力対応情報
/// </summary>
struct InputState
{
	PeripheralType type;//入力された機器の種別
	int id;//入力情報が入る(キーボードの場合はインデックス、パッドの場合はビット)
};

/// <summary>
/// 入力を抽象化するためのクラス
/// </summary>
class Input
{
	//mapは対応表のようなもの
private:
	std::map<std::string, std::vector<InputState>> m_inputTable;///イベント名と実際の入力の対応表
	std::map<std::string, bool>m_inputData;///実際に入力されたかどうかのデータ
	std::map<std::string, bool>m_lastInputData;///前のフレームに入力されたかどうかのデータ

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Input();

	/// <summary>
	/// 毎フレーム呼び出して、
	/// 入力情報を更新させる
	/// </summary>
	void Update();

	/// <summary>
	/// 特定のボタンがおされているか
	/// </summary>
	/// <param name="name">イベント名(例:OK,BACKなど)</param>
	/// <returns>押されている:true,押されていない:false</returns>
	bool IsPressed(const char* name) const;

	/// <summary>
	/// 特定のボタンが現在押されたか(押された瞬間のみ反応する)
	/// </summary>
	/// <param name="name">イベント名</param>
	/// <returns>今押された瞬間:true,押されてないor押しっぱなし:false</returns>
	bool IsTriggered(const char* name) const;

	/// <summary>
	/// 特定のボタンが離された瞬間のみ反応する
	/// </summary>
	/// <param name="name">イベント名</param>
	/// <returns>離された瞬間:true,まだ押されていたらfalse</returns>
	bool IsReleased(const char* name) const;
};

