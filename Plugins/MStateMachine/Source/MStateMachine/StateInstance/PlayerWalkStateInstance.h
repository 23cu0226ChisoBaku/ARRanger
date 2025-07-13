// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerWalkStateInstance.generated.h"

/**
 * ƒvƒŒƒCƒ„[‚Ì•à‚«ó‘Ô
 */
UCLASS()
class MSTATEMACHINE_API UPlayerWalkStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

};
