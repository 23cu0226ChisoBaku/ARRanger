// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/ARGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UARGameInstance::ResetGame()
{
  if (UWorld* world = GetWorld())
  {
    if (world->IsGameWorld())
    {
      UGameplayStatics::OpenLevel(this, ResetGameLevelName);
    }
  }
}