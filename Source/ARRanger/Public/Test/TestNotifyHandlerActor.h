// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "TestNotifyHandlerActor.generated.h"

UCLASS()
class ARRANGER_API ATestNotifyHandlerActor : public AActor,
                                             public IObservableSubjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestNotifyHandlerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable, Category = "ARRanger|UnitTest|PlayerObservation")
  void TestJumpEvent();

};
