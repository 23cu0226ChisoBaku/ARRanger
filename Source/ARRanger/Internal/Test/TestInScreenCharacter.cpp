// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Test/TestInScreenCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATestInScreenCharacter::ATestInScreenCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestInScreenCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestInScreenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  if (Target != nullptr)
  {
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC == nullptr)
    {
      return;
    }
  
    FVector2D screenLoc{};
    const bool bProjected = PC->ProjectWorldLocationToScreen
                          (
                            Target->GetActorLocation(),
                            screenLoc,
                            true        // bPlayerViewportRelative
                          );
    if (!bProjected)
    {
      return;
    }
  
    int32 viewportSizeX, viewportSizeY;
    PC->GetViewportSize(viewportSizeX, viewportSizeY);
  
    const bool bInScreen = (screenLoc.X >= 0.0 && screenLoc.X <= (double)viewportSizeX) && (screenLoc.Y >= 0.0 && screenLoc.Y <= (double)viewportSizeY);
  
    if (bInScreen)
    {
  
    }
  }

}

// Called to bind functionality to input
void ATestInScreenCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

