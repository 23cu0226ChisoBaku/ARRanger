// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARPhysicsEngineTest.h"

// Sets default values
AARPhysicsEngineTest::AARPhysicsEngineTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AARPhysicsEngineTest::BeginPlay()
{
	Super::BeginPlay();

  SetMagnetismType(EARMagnetismType::Attraction);
	
}

// Called every frame
void AARPhysicsEngineTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARPhysicsEngineTest::TestRequestMagneticTask(AActor* Target)
{
  if ((Target != nullptr) && Target->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
  {
    Physics_RegisterMagneticTask(this, ::Cast<IARMagnetizableInterface>(Target));
  }
}

void AARPhysicsEngineTest::OnAttractionEvaluated(const FARMagneticForceResult& Result)
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Attraction Callback!!!"));
  }
}

void AARPhysicsEngineTest::OnRepulsionEvaluated(const FARMagneticForceResult& Result)
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Repulsion Callback!!!"));
  }
}

