// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateInstance.generated.h"

/**
 * 
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
