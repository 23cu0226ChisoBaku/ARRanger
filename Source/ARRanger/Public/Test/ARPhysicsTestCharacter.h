// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"

#include "IARMagnetizableInterface.h"
#include "Physics/IARPhysicsSystemHost.h"
#include "ARPhysicsTestCharacter.generated.h"

UCLASS()
class ARRANGER_API AARPhysicsTestCharacter : public ACharacter,
                                             public IARMagnetizableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARPhysicsTestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) override { }
	virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override { }
	virtual AActor* GetActor() override { return this; }

  UFUNCTION(BlueprintCallable)
  void SetAttra();

  UFUNCTION(BlueprintCallable)
  void SetRepul();
};
