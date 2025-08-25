// Fill out your copyright notice in the Description page of Project Settings.


#include "ARRepositoryAssetHelper.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"

#include "Internal/ARLoggingHeader.h"

void UARRepositoryAssetHelper::SaveMagneticParametersAsset(FString AssetPath)
{
  UObject* asset = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath);

  if (asset == nullptr)
  {
    UE_LOG(LogAREditorAsset, Error, TEXT("Asset is invalid. Asset path: [%s]"), *AssetPath);
    return;
  }

  UPackage* package = asset->GetPackage();

  if (package == nullptr)
  {
    UE_LOG(LogAREditorAsset, Error, TEXT("Package is invalid. Asset path: [%s]"), *AssetPath);
    return;
  }

  // Save asset
  UEditorLoadingAndSavingUtils::SavePackages({ package }, false);
}

UObject* UARRepositoryAssetHelper::GetAssetByClass(FString AssetPath, UClass* AssetClass, UFactory* AssetFactory, bool bCreateIfNotFound)
{
  if ((AssetClass == nullptr))
  {
    UE_LOG(LogAREditorAsset, Error, TEXT("Asset class is invalid"));
    return nullptr;
  }

  // Load asset
  UObject* asset = ::StaticLoadObject(AssetClass, nullptr, *AssetPath);

  // Create one if asset is not exist and bCreateIfNotFound is true
  if ((asset == nullptr) && bCreateIfNotFound)
  {

    FAssetToolsModule& assetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    IAssetTools& assetTools = assetToolsModule.Get();

    // Check AssetFactory is able to create the desired asset
    if ((AssetFactory == nullptr) || AssetFactory->SupportedClass != AssetClass)
    {
      UE_LOG(LogAREditorAsset, Error, TEXT("Can not create asset with given UFactory"));
      return nullptr;
    }
    // Create asset
    const FString assetName = FPaths::GetBaseFilename(AssetPath);
    asset = assetTools.CreateAsset(assetName, FPaths::GetPath(AssetPath), AssetClass, AssetFactory);
  
    // Create asset failed
    if (asset == nullptr)
    {
      UE_LOG(LogAREditorAsset, Error, TEXT("Create asset failed"));
    }
  }

  return asset;
}