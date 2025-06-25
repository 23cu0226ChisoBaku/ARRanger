// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimMontageHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MLIBRARY_API UAnimMontageHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static float GetSectionDuration(UAnimInstance*, UAnimMontage*, FName);
};
