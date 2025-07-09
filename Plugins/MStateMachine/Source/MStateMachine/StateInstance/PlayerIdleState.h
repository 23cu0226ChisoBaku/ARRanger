// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerIdleState.generated.h"

/**
 * 
 */
UCLASS()
class MSTATEMACHINE_API UPlayerIdleState : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	UPlayerIdleState(const FObjectInitializer & = FObjectInitializer::Get());

};
