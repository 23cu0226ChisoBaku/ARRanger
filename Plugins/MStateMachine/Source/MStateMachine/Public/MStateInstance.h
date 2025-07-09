// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine の 軽量な UObject ベース型を定義するためのヘッダ
// USTRUCT() や UCLASS() を使ったクラス・構造体を作るときに使う
#include "UObject/NoExportTypes.h"

// FGameplayTag, FGameplayTagContainer などの Gameplay Tag システムを使うためのヘッダ
// UE の標準的なキーワードによる状態・機能の識別システム
#include "GameplayTagContainer.h"

// LogMState と LogMStateMachine というログカテゴリ実体（定義）を行っている
#include "MStateMachineLogChannels.h"

#include "MStateInstance.generated.h"

/**
 * ステートの本体
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class MSTATEMACHINE_API UMStateInstance : public UObject
{
	GENERATED_BODY()

public:
	UMStateInstance(const FObjectInitializer& = FObjectInitializer::Get());
	virtual void BeginDestroy() override;

public:
	virtual void EntryState();
	virtual void TickState(float inDeltaTime);
	virtual void ExitState();

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "EntryState"))
	void K2_EntryState();

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "TickState"))
	void K2_TickState(float InDeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "ExitState"))
	void K2_ExitState();
};
