// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticParamEdit/MagneticParamAssetSavePathFactory.h"

#include "MagneticParamAssetPathSave.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MagneticParamAssetSavePathFactory)

UMagneticParamAssetSavePathFactory::UMagneticParamAssetSavePathFactory()
{
  DataAssetClass = UMagneticParamAssetPathSave::StaticClass();
  SupportedClass = DataAssetClass;
  bCreateNew = true;
}

bool UMagneticParamAssetSavePathFactory::ShouldShowInNewMenu() const 
{
  // Never shows this in menu
  return false;
}