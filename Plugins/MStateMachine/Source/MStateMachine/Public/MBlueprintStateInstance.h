// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MStateInstance.h"

#include "MBlueprintStateInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UMBlueprintStateInstance : public UMStateInstance
{
	GENERATED_BODY()

  public:
    /**Start UObject Interface */

    virtual UWorld* GetWorld() const override;

    /**End UObject Interface */
    
  public:
    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "EnterState"))
    MSTATEMACHINE_API void K2_BlueprintEnterState(const FStateTransitionParameters& TransParams);

    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "TickState"))
    MSTATEMACHINE_API void K2_BlueprintTickState(const FStateTickParameters& TickParams);

    UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "ExitState"))
    MSTATEMACHINE_API void K2_BlueprintExitState(const FStateTransitionParameters& TransParams);

  private:
    void OnEnterState(const FStateTransitionParameters& TransParams) override final;
    void OnTickState(const FStateTickParameters& TickParams) override final;
    void OnExitState(const FStateTransitionParameters& TransParams) override final;
};
