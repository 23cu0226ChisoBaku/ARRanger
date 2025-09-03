// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "TestRepulsionBurstComponent.generated.h"

DECLARE_DELEGATE(OnBurstTransition);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UTestRepulsionBurstComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestRepulsionBurstComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


  UFUNCTION(BlueprintCallable)
  void StartBurst(AActor* StartTarget);

  UFUNCTION(BlueprintCallable)
  void EndBurst();

public:

  UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1", ClampMax = "10"))
  int32 TargetNum;

  UPROPERTY(EditDefaultsOnly)
  float BurstRadius;

  UPROPERTY(EditDefaultsOnly)
  float BurstDashSpeed;

  UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.0f))
  float BurstFinishActionInterval;

public:

  OnBurstTransition OnStartBurst;
  OnBurstTransition OnEndBurst;

private:
  TSet<TObjectPtr<AActor>> TargetActors;
		
};
