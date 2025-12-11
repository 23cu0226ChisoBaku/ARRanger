// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Test/StaticRepulsionActor.h"
#include "Components/BoxComponent.h"

// Sets default values
AStaticRepulsionActor::AStaticRepulsionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

  BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
  SetMagnetismType(EARMagnetismType::Repulsion);
}

void AStaticRepulsionActor::OnRepulsionEvaluated(const FARMagneticForceResult& Result)
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("On Movable repulsion actor"));
  }

  if (BoxComp != nullptr)
  {
    BoxComp->AddImpulse((Result.FinalForce  * 100.0) + FVector{0.0,0.0,100000.0});
  }
} 


