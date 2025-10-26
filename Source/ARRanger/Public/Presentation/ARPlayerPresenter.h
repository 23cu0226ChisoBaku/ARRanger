// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "ARPlayerPresenter.generated.h"

class AARRangerCharacter;
class UARHealthComponent;
enum class EARMagnetismType : uint8;

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
  FARPlayerModel();

  void Initialize(AARRangerCharacter* InViewCharacter);

  void Reset();

  UPROPERTY()
  TObjectPtr<UARHealthComponent> HealthComponent;

  FVector ChargeStartFaceDir; 

  UPROPERTY(EditAnywhere, Category = "PlayerModel|Knockback")
  double LaunchPower = 400.0;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|Charge")
  double ChargeRotateHalfRange = 60.0;

  uint8 bIsCharging : 1;
  uint8 bIsInAir : 1;

};

UCLASS(Blueprintable, BlueprintType)
class UARPlayerPresenter : public UObject
{
	GENERATED_BODY()

public:

  UE_API void Initialize(AARRangerCharacter* InViewCharacter);

  UE_API void Deinitialize();

  UE_API void Input_HandleLeftStick(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  UE_API void Input_HandleTransform();

  UE_API void HandleChargeStart(); 

  UE_API void HandleChargeEnd();

private:

  void HandleCharacterMove(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  void HandleCharacterChargeRotate(double InX, double InY);

  void HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult);

  void HandleBattleStateChange(bool bIsInBattle);

  void HandleTransformedEvent(EARMagnetismType InNewTransformation);

  UFUNCTION()
  void OnGroundLanded(const FHitResult& InHit);

  void OnCharacterJumpStarted();

  void OnCharacterJumpStopped();

private:

  UPROPERTY()
  TObjectPtr<AARRangerCharacter> ViewCharacter;
	
  UPROPERTY(EditAnywhere)
  FARPlayerModel Model;

};

#undef UE_API