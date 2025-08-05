//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#pragma once

// 前方宣言
class AActor;
class UWorld;
class ABlinkOutlineTickActor;

class BlinkingOutlineSystem
{
public:

	/*
	* @brief アウトラインの点滅処理を行うアクターの生成
	*/
	void CreateTickingActor(UWorld* world);

	/*
	* @brief LineTraceSingleARObjectComponent のデリゲート関数に登録
	*/
	void RegisterDelegateFunction();

	/*
	* @brief アウトラインの処理を呼び出すデリゲート関数
	*
	* @param アウトラインをつける対象のオブジェクトポインタ
	* 
	*  AGameModeBaseを継承しているクラスのStartPlay()関数でバインド
	*/
	void SetTargetMagnetizableObjectDelegate(AActor* magnetizableObject);

private:	

	/*
	* @brief BlinkOutlineTickActor に点滅処理をリクエスト
	*/
	void AddBlinkingOutlineObject(AActor* magnetizableObject);

private:
	ABlinkOutlineTickActor* m_TickActor;	// 点滅処理を駆動するためのアクター
};

