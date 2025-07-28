//*************************************************
// 斥力引力の挙動を発動させるためのインターフェース
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface/IARTypeInterface.h"
#include "IARObjectInterface.generated.h"

/*
* BP継承可能インターフェース
*/
UINTERFACE(Blueprintable)
class UARObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/*
* IAROcjectInterface 
*/
class IARObjectInterface
{
	GENERATED_BODY()

public:
	virtual void OnAttraction() = 0;	// 引力処理
	virtual void OnRepulsion() = 0;		// 斥力処理
	virtual EARType GetCurrentARType() const = 0;				// 現在の状態を取得する関数
	virtual EARType SetNewARType(EARType newType) const = 0;	// 新しい状態を設定する関数

};