// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "ARGuidancePoint.generated.h"


#define UE_API ARRANGER_API

/**Forward declaration */
enum class EARMagnetismType : uint8;
class UPrimitiveComponent;
class UBoxComponent;

UCLASS(Abstract)
class AARGuidancePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UE_API AARGuidancePoint();

protected:
  // Called when the game starts or when spawned
  UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
  // Called every frame
  UE_API virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable)
  UE_API void SetChildPoint(AARGuidancePoint* InChild);

  UFUNCTION(BlueprintCallable, Category = "Guidance|Visibility")
  UE_API virtual void OnMagnetismTypeChanged(EARMagnetismType Type);

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Visibility", meta = (DisplayName = "OnAttractionChanged"))
  UE_API void K2_OnAttractionChanged();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Visibility", meta = (DisplayName = "OnRepulsionChanged"))
  UE_API void K2_OnRepulsionChanged();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance", meta = (DisplayName = "OnTerminationStarted"))
  UE_API void K2_OnTerminationStarted();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance", meta = (DisplayName = "OnTerminationEventUpdated"))
  UE_API void K2_OnTerminationEventUpdated(float DeltaTime);

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance", meta = (DisplayName = "OnTerminationEnded"))
  UE_API void K2_OnTerminationEnded();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance", meta = (DisplayName = "OnActivationSet"))
  UE_API void K2_OnActivationSet(bool bActive);

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Animation", meta = (DisplayName = "OnGuidanceAnimationUpdated"))
  UE_API void K2_OnGuidanceAnimationUpdated(const FVector& Origin, const FVector& Destination, float AnimTotalLength, float AnimCurrentTime, float DeltaTime);

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Animation", meta = (DisplayName = "OnGuidanceAnimationEnded"))
  UE_API void K2_OnGuidanceAnimationEnded();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Animation", meta = (DisplayName = "OnGuidanceAnimationReset"))
  UE_API void K2_OnGuidanceAnimationReset();

  UFUNCTION(BlueprintImplementableEvent, Category = "Guidance|Animation", meta = (DisplayName = "OnChildPointSet"))
  UE_API void K2_OnChildPointSet(AARGuidancePoint* InChild);

  UFUNCTION(BlueprintCallable, Category = "Guidance")
  UE_API void OnTerminationEnded();

  UFUNCTION(BlueprintPure, Category = "Guidance")
  UE_API bool IsTerminationTriggered() const;

  UFUNCTION(BlueprintPure, Category = "Guidance|Animation")
  UE_API bool CanPlayGuidanceAnimation() const;

  UFUNCTION(BlueprintCallable, Category = "Guidance|Animation")
  UE_API void UpdateGuidanceAnimation(float DeltaTime);

  UFUNCTION(BlueprintCallable, Category = "Guidance|Animation")
  UE_API void ResetGuidanceAnimation();
  
  UE_API void SetActive(bool bActive);

private:
  UFUNCTION()
  void OnGuidancePointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  void OnGuidanceAnimationUpdated(const AARGuidancePoint& ChildPoint, float DeltaTime);

  void ResetAnim();

  void UpdateAnim(const FVector& Destination, float DeltaTime);

  void StopAnim();

  void OnChildPointSet(AARGuidancePoint* InParentPoint);

private:
  
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guidance|Collision", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UBoxComponent> GuidancePointCollision;

  UPROPERTY(EditAnywhere)
  TObjectPtr<AARGuidancePoint> ChildPoint;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guidance|Animation", meta = (AllowPrivateAccess = "true", ClampMin = 0))
  float GuidanceAnimLength;

  UPROPERTY(EditAnywhere, Category = "Guidance|Animation", meta = (ClampMin = 0))
  float GuidanceAnimDelay;
  
  float m_guidanceAnimTimeCnt;

  FTimerHandle m_guidanceAnimDelayTimerHandle;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guidance|Animation", meta = (AllowPrivateAccess = "true", DisplayName = "Loop Animation"))
  bool bLoopAnim;

  uint8 bCanTerminationEventUpdate : 1;

  uint8 bIsAnimDelaying : 1;
};

#undef UE_API
