#include "MagneticParamAssetPathSave.h"

void UMagneticParamAssetPathSave::Save(const FString& NewAssetPath, bool bForceUpdateAssetView)
{
  if (MagneticParamListAssetPath == NewAssetPath)
  {
    UE_LOG(LogTemp, Warning, TEXT("Will not save because path is same.Path: [%s]"), *NewAssetPath);
    return;
  }

  MagneticParamListAssetPath = NewAssetPath;

  if (bForceUpdateAssetView)
  {
    TargetAsset_ViewOnly = TSoftObjectPtr{ FSoftObjectPath{MagneticParamListAssetPath} };
  }
}

FString UMagneticParamAssetPathSave::GetAssetPath() const
{
  return MagneticParamListAssetPath;
}