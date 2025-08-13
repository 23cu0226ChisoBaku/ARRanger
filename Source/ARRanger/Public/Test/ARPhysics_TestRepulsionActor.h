// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IARMagnetizableInterface.h"
#include "Physics/IARPhysicsSystemHost.h"

#include "ARPhysics_TestRepulsionActor.generated.h"

class UCapsuleComponent;

UCLASS()
class ARRANGER_API AARPhysics_TestRepulsionActor : public AActor,
                                                   public IARMagnetizableInterface,
                                                   public IARPhysicsSystemHost
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARPhysics_TestRepulsionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual AActor* GetActor() { return this; }


  UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> Capsule;
};
