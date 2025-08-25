// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/DataAssetFactory.h"
#include "MagneticParamAssetSavePathFactory.generated.h"

/**
 * 
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UMagneticParamAssetSavePathFactory : public UDataAssetFactory
{
	GENERATED_BODY()

public:
  UMagneticParamAssetSavePathFactory();

  /**Start UFactory interface */
  virtual bool ShouldShowInNewMenu() const override;
  /**End UFactory interface */
};
