// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "ARPlayerPresenter.generated.h"

class AARRangerCharacter;
class UARHealthComponent;

namespace ARRanger
{

namespace Battle
{
  struct FARDamageResult;
} // namespace ARRanger::Battle

} // namespace ARRanger

#define UE_API ARRANGER_API

USTRUCT()
struct FARPlayerModel
{
  GENERATED_BODY()

public:

  UPROPERTY()
  TObjectPtr<UARHealthComponent> HealthComponent;

  UPROPERTY(EditAnywhere)
  double LaunchPower = 400.0;

};

UCLASS(Blueprintable, BlueprintType)
class UARPlayerPresenter : public UObject
{
	GENERATED_BODY()

public:
  UE_API void Initialize(AARRangerCharacter* InViewCharacter);

  UE_API void Deinitialize();

  UE_API void HandleMoveInput(double InRight, double InForward, /**TODO */double InDeadZone);

private:
  void HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult);

  void HandleBattleStateChange(bool bIsInBattle);
private:

  UPROPERTY()
  TObjectPtr<AARRangerCharacter> View;
	
  UPROPERTY(EditAnywhere)
  FARPlayerModel Model;

};

#undef UE_API