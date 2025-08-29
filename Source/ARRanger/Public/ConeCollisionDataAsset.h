//*************************************************
// ConeCollision のデータアセット
//*************************************************

#pragma once

#include "Engine/DataAsset.h"
#include "ConeCollisionDataAsset.generated.h"

UCLASS(Const)
class UConeCollisionDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

  /*コリジョン範囲(円錐の高さ)*/
  UPROPERTY(EditDefaultsOnly)							
  float Height;                   
  /*コリジョンの傾き*/
  UPROPERTY(EditDefaultsOnly)
  float ConeAngle;
};