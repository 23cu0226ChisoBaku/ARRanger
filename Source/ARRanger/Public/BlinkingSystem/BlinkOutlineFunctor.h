//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#pragma once

// 前方宣言
struct FBlinkingActorData;

class BlinkOutlineFunctor
{
public:
	BlinkOutlineFunctor();
	~BlinkOutlineFunctor();

	/**
	 * @brief 点滅処理を行う関数
	 */
	void OutlineBlink(FBlinkingActorData* blinkingData); 

private:
	
};
