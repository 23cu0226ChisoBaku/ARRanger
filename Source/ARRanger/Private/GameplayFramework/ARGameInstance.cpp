// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/ARGameInstance.h"
#include "Kismet/GameplayStatics.h"

namespace
{
  void ResetToLevel(const UObject* WorldContextObject, FName LevelName);
}

void UARGameInstance::ResetGame()
{
  if (OnReset.IsBound())
  {
    OnReset.Broadcast();
  }
  
  ResetToLevel(this, ResetGameLevelName);

}

void UARGameInstance::ResetBattleTestStage()
{
  if (OnReset.IsBound())
  {
    OnReset.Broadcast();
  }

  ResetToLevel(this, ResetBattleTestStageLevelName);
  
}

void UARGameInstance::SetColorBlindMode(bool bEnabled)
{
  if (OnColorBlindModeEnabled.IsBound())
  {
    OnColorBlindModeEnabled.Broadcast(bEnabled);
  }

  bColorBlindModeOn = bEnabled;
}

bool UARGameInstance::IsColorBlindMode() const
{
  return bColorBlindModeOn;
}

namespace
{
  void ResetToLevel(const UObject* WorldContextObject, FName LevelName)
  {
    if (GEngine != nullptr)
    {
      UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); 
      if (world != nullptr)
      {
        if (world->IsGameWorld())
        {
          UGameplayStatics::OpenLevel(WorldContextObject, LevelName);
        }
      }
    }
  }
}