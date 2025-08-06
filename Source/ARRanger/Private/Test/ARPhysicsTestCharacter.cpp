// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARPhysicsTestCharacter.h"

// Sets default values
AARPhysicsTestCharacter::AARPhysicsTestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AARPhysicsTestCharacter::BeginPlay()
{
	Super::BeginPlay();
  SetMagnetismType(EARMagnetismType::Attraction);
	
}

// Called every frame
void AARPhysicsTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AARPhysicsTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AARPhysicsTestCharacter::SetAttra()
{
  SetMagnetismType(EARMagnetismType::Attraction);
}

void AARPhysicsTestCharacter::SetRepul()
{
  SetMagnetismType(EARMagnetismType::Repulsion);
}

