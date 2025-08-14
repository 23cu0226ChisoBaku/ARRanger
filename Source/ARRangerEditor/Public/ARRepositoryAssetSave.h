// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARRepositoryAssetSave.generated.h"

UCLASS()
class ARRANGEREDITOR_API UARRepositoryAssetSave : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

  public:

    UFUNCTION(BlueprintCallable)
    static void SaveMagneticParametersAsset(FString AssetPath, bool& bOutSuccess, FString& OutMessage);
	
};
