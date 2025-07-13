// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MStateDefinition.h"
#include "PlayerIdleStateDefinition.generated.h"

/**
 *  プレイヤの待機状態の定義
 */
UCLASS()
class MSTATEMACHINE_API UPlayerIdleStateDefinition : public UMStateDefinition
{
	GENERATED_BODY()

public:
	UPlayerIdleStateDefinition();
};
