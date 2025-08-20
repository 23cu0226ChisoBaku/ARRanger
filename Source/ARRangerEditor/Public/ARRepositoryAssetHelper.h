// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARRepositoryAssetHelper.generated.h"

UCLASS()
class ARRANGEREDITOR_API UARRepositoryAssetHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

  public:

    UFUNCTION(BlueprintCallable)
    static void SaveMagneticParametersAsset(FString AssetPath);

    UFUNCTION(BlueprintCallable)
    [[nodiscard]] static UObject* GetAssetByClass(FString AssetPath, UClass* AssetClass, UFactory* AssetFactory = nullptr, bool bCreateIfNotFound = true);
	
};
