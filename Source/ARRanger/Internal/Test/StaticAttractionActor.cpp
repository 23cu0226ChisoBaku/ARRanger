// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Test/StaticAttractionActor.h"
#include "Components/SphereComponent.h"


// Sets default values
AStaticAttractionActor::AStaticAttractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

  SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
  SetMagnetismType(EARMagnetismType::Attraction);
}

void AStaticAttractionActor::OnAttractionEvaluated(const FARMagneticForceResult& Result)
{
  // Do nothing
}

