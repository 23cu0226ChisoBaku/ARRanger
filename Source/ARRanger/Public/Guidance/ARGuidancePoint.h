// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "ARGuidancePoint.generated.h"


#define UE_API ARRANGER_API

UENUM(BlueprintType)
enum struct EGuidancePointVisibilityType : uint8
{
  Visible_Attraction,
  Visible_Repulsion,
  Visible_Both,
};

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

  UFUNCTION(BlueprintCallable, Category = "Guidance")
  UE_API void OnTerminationEnded();

  UFUNCTION(BlueprintPure, Category = "Guidance")
  UE_API bool IsTerminationTriggered() const;

  UE_API void SetActive(bool bActive);

private:
  UFUNCTION()
  UE_API void OnGuidancePointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
  
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guidance|Collision", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UBoxComponent> GuidancePointCollision;

  UPROPERTY(EditDefaultsOnly, Category = "Guidance|Visibility")
  EGuidancePointVisibilityType Visibility;

  uint8 bCanTerminationEventUpdate : 1;
};

#undef UE_API
