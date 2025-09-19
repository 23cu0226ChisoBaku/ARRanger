#pragma once

#include "Enemy/Enemy_Zako.h"

#include "Enemy_Shooter.generated.h"

UCLASS()
class ARRANGER_API AEnemy_Shooter : public AEnemy_Zako
{
  GENERATED_BODY()

public:
  AEnemy_Shooter();

protected:

  /**Start AActor Interface */
  virtual void BeginPlay() override;
  /**End AActor Interface */

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  TSubclassOf<AActor> BeamProjectileClass;

  UPROPERTY(VisibleAnywhere, Category = "Combat")
  TObjectPtr<USceneComponent> MuzzlePoint;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float IdealRange;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float RangeTolerance;

};
