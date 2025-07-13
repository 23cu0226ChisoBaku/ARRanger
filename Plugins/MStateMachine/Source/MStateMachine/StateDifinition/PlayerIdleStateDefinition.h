// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MStateDefinition.h"
#include "PlayerIdleStateDefinition.generated.h"

/**
 *  �v���C���̑ҋ@��Ԃ̒�`
 */
UCLASS()
class MSTATEMACHINE_API UPlayerIdleStateDefinition : public UMStateDefinition
{
	GENERATED_BODY()

public:
	UPlayerIdleStateDefinition();
};
