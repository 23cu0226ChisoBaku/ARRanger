// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/ARGameInstance.h"
#include "Kismet/GameplayStatics.h"

namespace
{
  void LoadLevel(const UObject* WorldContextObject, FName LevelName);
}

void UARGameInstance::ResetGame()
{
  if (OnReset.IsBound())
  {
    OnReset.Broadcast();
  }
  
  LoadLevel(this, ResetGameLevelName);

}

void UARGameInstance::ResetBattleTestStage()
{
  if (OnReset.IsBound())
  {
    OnReset.Broadcast();
  }

  LoadLevel(this, ResetBattleTestStageLevelName);
  
}

void UARGameInstance::ProcessGameClear()
{
  LoadLevel(this, GameClearLevelName);
}

void UARGameInstance::ProcessGameOver()
{
  LoadLevel(this, GameOverLevelName);
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
  void LoadLevel(const UObject* WorldContextObject, FName LevelName)
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