// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateInstance.generated.h"

/**前方宣言 */
class UMStateMachineComponent;

#define UE_API MSTATEMACHINE_API

/**
 * @brief ステート遷移種類
 */
UENUM(BlueprintType)
enum class EStateTransitionType : uint8
{
  Enter,
  Exit,
};

/**
 * @brief ステート遷移パラメーター
 */
USTRUCT(BlueprintType)
struct FStateTransitionParameters
{
  GENERATED_BODY()

  /**
   * @brief 遷移タグ
   * 
   * Enterの場合は遷移する前のステートのタグ
   * 
   * Exitの場合は遷移する先のステートのタグ
   */
  UPROPERTY(BlueprintReadOnly)
  FGameplayTag TransitionTag = FGameplayTag::EmptyTag;

  /**
   * @brief 遷移状態
   */
  UPROPERTY(BlueprintReadOnly)
  EStateTransitionType Transition = EStateTransitionType::Enter;

  /**
   * @brief コンテキスト
   */
  TSharedPtr<class FMStateContext, ESPMode::NotThreadSafe> Context;
};


/**
 * @brief ステート更新パラメーター
 */
USTRUCT(BlueprintType)
struct FStateTickParameters
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly)
  float DeltaTime = 0.0f;

  /**
   * @brief コンテキスト
   */
  TSharedPtr<class FMStateContext, ESPMode::NotThreadSafe> Context;
};


/**
 * ステート初期化パラメータ
 */
struct FStateInitializationParameters
{
  /**
   * @brief ステートを持つActor
   */
  TObjectPtr<AActor> OwnerActor;

  /**
   * @brief このステートを管理するステートマシンコンポーネント
   */
  TObjectPtr<UMStateMachineComponent> OwnerStateMachineComponent;
};


/**
 * @brief ステート解放パラメーター
 */
struct FStateUninitializationParameters
{
  // TODO: 拡張する可能性がある
};



/**
 * ステートインスタンス
 */
UCLASS(Abstract, MinimalAPI)
class UMStateInstance : public UObject
{
	GENERATED_BODY()

public:
	UE_API UMStateInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief ステートを初期化する
   * 
   * @param InitParams 初期化パラメーター
   */
  UE_API void InitializeState(const FStateInitializationParameters& InitParams);

  /**
   * @brief ステートを解放する
   * 
   * @param UninitParams 解放パラメーター
   */
  UE_API void UninitializeState(const FStateUninitializationParameters& UninitParams);

  /**
   * @brief ステートに入る
   * 
   * @param TransParams ステート遷移パラメーター
   */
	UE_API void EnterState(const FStateTransitionParameters& TransParams);

  /**
   * @brief ステートを更新
   * 
   * @param TickParams ステート更新パラメーター
   */
	UE_API void TickState(const FStateTickParameters& TickParams);

  /**
   * @brief ステートを抜ける
   * 
   * @param TransParams ステート遷移パラメーター
   */
	UE_API void ExitState(const FStateTransitionParameters& TransParams);
  
protected:

  /**
   * ステートインスタンス実装メソッド
   * 子クラスで必要なメソッドをオーバーライド
   */

  UE_API virtual void OnInitializeState(const FStateInitializationParameters& InitParams) { };
  UE_API virtual void OnUninitializeState(const FStateUninitializationParameters& UninitParams) { };
  UE_API virtual void OnEnterState(const FStateTransitionParameters& TransParams) { };
  UE_API virtual void OnTickState(const FStateTickParameters& TickParams) { };
  UE_API virtual void OnExitState(const FStateTransitionParameters& TransParams) { };
	
};

#undef UE_API
