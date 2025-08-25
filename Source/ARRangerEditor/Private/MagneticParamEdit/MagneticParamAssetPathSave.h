// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MagneticParamAssetPathSave.generated.h"

UCLASS()
class UMagneticParamAssetPathSave : public UDataAsset
{
	GENERATED_BODY()

private:
  UPROPERTY(VisibleAnywhere, Category = "Editor|AssetSaveDatas")
  FString MagneticParamListAssetPath;

  UPROPERTY(VisibleAnywhere, Category = "Editor|AssetSaveDatas")
  TSoftObjectPtr<class UDataAsset> TargetAsset_ViewOnly;

public:
  void Save(const FString& NewAssetPath, bool bForceUpdateAssetView = true);

  FString GetAssetPath() const;
};
