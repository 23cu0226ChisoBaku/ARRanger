// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "Physics/IARPhysicsSystemHost.h"

#include "ARPlayerPresenter.generated.h"

class AARRangerCharacter;
class UARHealthComponent;
enum class ECameraRigType : uint8;
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

  FVector ClimbSurfaceNormal;
  
  FVector SnapTargetImpactPoint;
  
  FVector SnapStartPosition;
  
  FRotator SnapStartRotation;

  FVector2D TargetSnapInputDirection; 
  
  UPROPERTY(EditAnywhere, Category = "PlayerModel|Knockback")
  double LaunchPower;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|Charge")
  double ChargeRotateHalfRange;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|TargetSnap")
  double TargetSnapDetectMaxRange;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|TargetSnap")
  float SnapTimeInterval;

  float SnapTimeCounter;

  TWeakObjectPtr<AActor> SnapTargetActor;

  uint8 bIsCharging : 1;
  uint8 bIsInAir : 1;
  uint8 bIsClimbing : 1;
  uint8 bIsInComboAction : 1;
  uint8 bCanUpdateSnapMovement : 1;
  uint8 bIsReadyToSearchSnapTarget : 1;
};

UCLASS(Blueprintable, BlueprintType)
class UARPlayerPresenter : public UObject,
                           public IARPhysicsSystemHost
{
	GENERATED_BODY()

public:

  UE_API void Initialize(AARRangerCharacter* InViewCharacter);

  UE_API void Deinitialize();

  UE_API void Input_HandleLeftStick(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  UE_API void Input_HandleTransform();

  UE_API void Input_HandleCameraReset();

  UE_API void HandleChargeStart(); 

  UE_API void HandleChargeEnd();

private:

  void HandleCharacterMove(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  void HandleSnapTargetUpdate(double InX, double InY);

  void RegisterSnapTargetTask();

  void UpdateSnapTarget(float DeltaTime);

  void UnregisterSnapTargetTask();
  
  void HandleCharacterChargeRotate(double InX, double InY);

  void HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult);

  void HandleBattleStateChange(bool bIsInBattle);

  void HandleTransformedEvent(EARMagnetismType InNewTransformation);

  void HandleAttackAbilityStarted();

  void HandleAttackAbilityEnded();

  void StartClimbing();

  void StopClimbing();

  void UpdateClimbing(float DeltaTime);

  UFUNCTION()
  void OnGroundLanded(const FHitResult& InHit);

  UFUNCTION()
  void OnClimbSurfaceOverlapBegan(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
  
  UFUNCTION()
  void OnClimbSurfaceOverlapEnded(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
  void OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
  void OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  UFUNCTION()
  void OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
  
  UFUNCTION()
  void OnCameraRigChanged(ECameraRigType InType);

  void OnCharacterJumpStarted();

  void OnCharacterJumpStopped();

  bool CanUpdateClimbingInternal() const;

  void StopSnapTargetInternal();

private:

  UPROPERTY()
  TObjectPtr<AARRangerCharacter> ViewCharacter;
	
  UPROPERTY(EditAnywhere)
  FARPlayerModel Model;

  FDelegateHandle Handle_UpdateClimbing{};

  FDelegateHandle Handle_UpdateSnapTarget{};
};

#undef UE_API