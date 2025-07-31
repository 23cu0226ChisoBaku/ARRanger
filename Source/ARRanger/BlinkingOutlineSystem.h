//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#pragma once

#include "Public/BlinkOutlineTickActor.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

// 前方宣言
class AActor;

class BlinkingOutlineSystem
{
	/*
	* @brief 
	*/
	AActor* CreateTickingActor();

	/*
	* @brief LineTraceSingleARObjectComponent のデリゲート関数に登録する
	*/
	void RegisterOnOutline();

	/*
	* @brief BlinkOutlineTickActor に点滅処理をリクエストする
	*/
	void RequestOnBlinkingOutLine();

private:
	ABlinkOutlineTickActor* TickActor;	// 点滅処理を駆動するためのアクター
};

