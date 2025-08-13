// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARPhysics_TestRepulsionActor.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AARPhysics_TestRepulsionActor::AARPhysics_TestRepulsionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	Capsule->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AARPhysics_TestRepulsionActor::BeginPlay()
{
	Super::BeginPlay();
  Capsule->OnComponentHit.AddDynamic(this, &AARPhysics_TestRepulsionActor::OnHit);

  SetMagnetismType(EARMagnetismType::Repulsion);
}

// Called every frame
void AARPhysics_TestRepulsionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARPhysics_TestRepulsionActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
  {
    Physics_RegisterMagneticTask_Once(this, magnetizableObj);
  }
}

