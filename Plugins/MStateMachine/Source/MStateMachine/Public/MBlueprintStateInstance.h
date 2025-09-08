// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MStateInstance.h"

#include "MBlueprintStateInstance.generated.h"

#define UE_API MSTATEMACHINE_API

/**
 * ブループリントステートインスタンス
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class UMBlueprintStateInstance : public UMStateInstance
{
	GENERATED_BODY()

  public:
    /**Start UObject Interface */

    UE_API virtual UWorld* GetWorld() const override;

    /**End UObject Interface */
    
  protected:
    /**
     * ステートに入るブループリントイベント
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "EnterState"))
    UE_API void K2_BlueprintEnterState(const FStateTransitionParameters& TransParams);

    /**
     * ステートを更新するブループリントイベント
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "TickState"))
    UE_API void K2_BlueprintTickState(const FStateTickParameters& TickParams);
    
    /**
     * ステートを抜けるブループリントイベント
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "ExitState"))
    UE_API void K2_BlueprintExitState(const FStateTransitionParameters& TransParams);

  private:
    void OnEnterState(const FStateTransitionParameters& TransParams) override final;
    void OnTickState(const FStateTickParameters& TickParams) override final;
    void OnExitState(const FStateTransitionParameters& TransParams) override final;
};

#undef UE_API