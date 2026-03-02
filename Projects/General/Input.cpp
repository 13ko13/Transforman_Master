#include "Input.h"
#include "DxLib.h"
#include "../Graphics/Fade.h"
#include "GameConstants.h"

Input::Input() :
	m_inputData{},
	m_lastInputData{},
	m_inputTable{}
{
	//イベント名を添え時にして、右辺値に実際の入力種別と押されたボタンの配列を置く
	m_inputTable["ok"] = { {PeripheralType::keyboard,KEY_INPUT_A},	//キーボード:エンターキー
							{PeripheralType::pad1,PAD_INPUT_A } };	//パッド:A

	m_inputTable["shot"] = { {PeripheralType::keyboard,KEY_INPUT_Z},	//キーボード:Z
							{PeripheralType::pad1,PAD_INPUT_C} };		//パッド:Xボタン

	m_inputTable["up"] = { {PeripheralType::keyboard,KEY_INPUT_UP},		//キーボード:上矢印
							{PeripheralType::pad1,PAD_INPUT_UP } };		//パッド:スティック上又は十字上

	m_inputTable["down"] = { {PeripheralType::keyboard,KEY_INPUT_DOWN},	//キーボード:下矢印
							{PeripheralType::pad1,PAD_INPUT_DOWN } };	//パッド:スティック下又は十字下

	m_inputTable["left"] = { {PeripheralType::keyboard,KEY_INPUT_LEFT},	//キーボード:左矢印
								{PeripheralType::pad1,PAD_INPUT_LEFT } };//パッド:スティック左又は十字左

	m_inputTable["right"] = { {PeripheralType::keyboard,KEY_INPUT_RIGHT},//キーボード:右矢印
							{PeripheralType::pad1,PAD_INPUT_RIGHT } };	 //パッド:スティック右又は十字右

	m_inputTable["jump"] = { {PeripheralType::keyboard,KEY_INPUT_SPACE},//キーボード:スペースキー
							{PeripheralType::pad1,PAD_INPUT_A	} };	//パッド:Aボタン

	m_inputTable["close"] = { {PeripheralType::keyboard,KEY_INPUT_ESCAPE},//キーボード:スペースキー
							{PeripheralType::pad1,PAD_INPUT_B	} };	//パッド:Bボタン

	m_inputTable["next"] = { {PeripheralType::keyboard,KEY_INPUT_E},//キーボード:Eキー
							{PeripheralType::pad1,PAD_INPUT_R	} };//パッド:R1ボタン

	m_inputTable["back"] = { {PeripheralType::keyboard,KEY_INPUT_Q},//キーボード:Qキー
							{PeripheralType::pad1,PAD_INPUT_L	} };//パッド:L1ボタン

	m_inputTable["parry"] = { {PeripheralType::keyboard,KEY_INPUT_X},//キーボード:Xキー
							{PeripheralType::pad1,PAD_INPUT_X	} };//パッド:Yボタン
#ifdef _DEBUG
	//デバッグ用キー
	//プレイヤーのステート切り替え
	m_inputTable["changeState(player)"] = { {PeripheralType::keyboard,KEY_INPUT_S & KEY_INPUT_P},//キーボード:SとP
							{PeripheralType::pad1,PAD_INPUT_X & PAD_INPUT_Y} };	//パッド:XとYボタン
	m_inputTable["changeState(enemy)"] = { {PeripheralType::keyboard,KEY_INPUT_S & KEY_INPUT_E},//キーボード:SとE
							{PeripheralType::pad1,PAD_INPUT_X & PAD_INPUT_A} };	//パッド:XとAボタン
	m_inputTable["playerSpeedUp"] = { {PeripheralType::keyboard,KEY_INPUT_W },//キーボード:W
							{PeripheralType::pad1,PAD_INPUT_X & PAD_INPUT_B } };//パッド:XボタンとBボタン
#endif // _DEBUG

	


	//あらかじめ入力データのための枠を開けておく
	//ここで枠を開けておかないと、チェックの際にAt関数でクラッシュする可能性がある(Null)
	for (const auto& inputInfo : m_inputTable)//forのinputInfoが変更されないようにconstにする
	{
		m_inputData[inputInfo.first] = false;
		m_lastInputData[inputInfo.first] = false;
	}
}

void Input::Update()
{
	//まず入力情報を取得
	char keyState[256];
	GetHitKeyStateAll(keyState);//生のキーボード情報
	int padState = GetJoypadInputState(DX_INPUT_PAD1);//生のPAD1情報
	m_lastInputData = m_inputData;//直前の入力を保存する(前のフレーム情報をコピー)
	//すべての入力イベントをチェックします
	//ここでinputDataが更新される
	//inputTableを回して各イベントの入力をチェックする
	for (const auto& inputInfo : m_inputTable)
	{
		auto& input = m_inputData[inputInfo.first];//inputInfo.firstには"ok"等が入っている
		//inputを書き換えると、inputDataのそのイベントが押されてるか同課を
		//書き換えることになる
		for (const auto& state : inputInfo.second)//InputStateのベクタを回す
		{
			//このループはInputState配列のループなので
			//まず、入力種別をチェックする
			switch (state.type)
			{
			case PeripheralType::keyboard://キーボードの場合
				//GetHitKeyStateAllでとってきた配列の中身を見て
				//入力されているかどうかをチェック
				input = keyState[state.id];
				break;
			case PeripheralType::pad1:
				//GetJoypadInputStateでとってきたビット情報を見て
				//登録されているビット情報と&を取り、そのビットが立っているかどうかを見る
				input = (padState & state.id);
				break;
			}
			if (input)	//大事
			{
				//breakしないとpad1がそのまま残ってしまう
				//最後のチェックで押されてないとfalseになってしまう
				break;
			}
		}
	}
}

bool Input::IsPressed(const char* name)const
{
	//もし「ない」イベントを送られるとクラッシュします
	//もしクラッシュがいやだったら
	//if(inputData.contains(name))
	//{
	//	 return false;//で回避できます。
	//}
	return m_inputData.at(name);//const関数内部なので[]ではなくatを使用してる
}

bool Input::IsTriggered(const char* name)const
{
	// 今入力されているボタンと
	// 前のフレームで入力されているボタンを比較する
	return m_inputData.at(name) && !m_lastInputData.at(name);
}

bool Input::IsReleased(const char* name) const
{
	//前のフレームで入力されているボタンと
	//現在入力されているかどうかを比較して
	//入力されていなかったらtrueにする
	return m_lastInputData.at(name) && !m_inputData.at(name);
}




