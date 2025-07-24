// Fill out your copyright notice in the Description page of Project Settings.

#include "Test/ARTestObserverGameModeBase.h"
#include "Test/TestNotifyHandler.h"
#include "PlayerObservation/IObservableSubjectInterface.h"


namespace Private
{
  void RawTest1()
  {
    UE_LOG(LogTemp, Error, TEXT("Raw test 1"));
  }

  void RawTest2()
  {
    UE_LOG(LogTemp, Error, TEXT("Raw test 2"));
  }
}

AARTestObserverGameModeBase::AARTestObserverGameModeBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  
} 

void AARTestObserverGameModeBase::SwitchToTest1State()
{
  if (!Handlers.Contains(Test1State))
  {
    return;
  }

  for (const auto& subject : Subjects)
  {
    if (subject.IsValid())
    {
      subject->UpdateNotifyHandler(Handlers[Test1State]);
    }
  }
}

void AARTestObserverGameModeBase::SwitchToTest2State()
{
  if (!Handlers.Contains(Test2State))
  {
    return;
  }

  for (const auto& subject : Subjects)
  {
    if (subject.IsValid())
    {
      subject->UpdateNotifyHandler(Handlers[Test2State]);
    }
  }
}

void AARTestObserverGameModeBase::StartPlay()
{
  Super::StartPlay();

  TSharedPtr<ARRanger::INotifyHandlerInterface> Test1Notify = ::MakeShared<ARRanger::Test::FTestNotifyHandler>();

}

void AARTestObserverGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
}

void AARTestObserverGameModeBase::OnGameModeBaseTest1()
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shit 1!!!!!"));
  }
}

void AARTestObserverGameModeBase::OnGameModeBaseTest2()
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Piss 2!!!!!"));
  }
}