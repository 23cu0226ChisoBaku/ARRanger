// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Test/MoveableAttractionActor.h"
#include "Components/SphereComponent.h"

// Sets default values
AMoveableAttractionActor::AMoveableAttractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

  SetMagnetismType(EARMagnetismType::Attraction);
  SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
}

void AMoveableAttractionActor::OnAttractionEvaluated(const FARMagneticForceResult& Result)
{
  SetActorLocation(GetActorLocation() + Result.FinalForce);
}
