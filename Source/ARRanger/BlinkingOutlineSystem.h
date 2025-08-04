//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#pragma once

#include "Public/BlinkOutlineTickActor.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

// 前方宣言
class AActor;
class UWorld;

class BlinkingOutlineSystem
{
	/*
	* @brief アウトラインの点滅処理を行うアクターの生成
	*/
	AActor* CreateTickingActor(const UWorld* world);

	/*
	* @brief アウトラインの処理を呼び出すデリゲート関数
	*
	* @param アウトラインをつける対象のオブジェクトポインタ
	* 
	*  AGameModeBaseを継承しているクラスのStartPlay()関数でバインド
	*/
	void OnBlinkingOutline(const AActor* magnetizableObject);

	/*
	* @brief LineTraceSingleARObjectComponent のデリゲート関数に登録
	*/
	void RegisterOnOutline();

	/*
	* @brief BlinkOutlineTickActor に点滅処理をリクエスト
	*/
	void RequestOnBlinkingOutLine();

private:
	ABlinkOutlineTickActor* TickActor;	// 点滅処理を駆動するためのアクター
};

