// Fill out your copyright notice in the Description page of Project Settings.

#include "Test/ARTestObserverGameModeBase.h"
#include "Test/TestNotifyHandler.h"
#include "PlayerObservation/IObservableSubjectInterface.h"

#include "Kismet/GameplayStatics.h"


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

  TSharedPtr<ARRanger::Test::FTestNotifyHandler> Test1Notify = ::MakeShared<ARRanger::Test::FTestNotifyHandler>();
  TSharedPtr<ARRanger::Test::FTestNotifyHandler> Test2Notify = ::MakeShared<ARRanger::Test::FTestNotifyHandler>();

  auto Test1ListNode = ARRanger::FObserverListRootNode::MakeListNode();
  Test1ListNode->Initialize();
  Test1ListNode->BindNewObserver(FSimpleDelegate::CreateStatic(&Private::RawTest1));
  Test1ListNode->BindNewObserver(FSimpleDelegate::CreateUObject(this, &AARTestObserverGameModeBase::OnGameModeBaseTest1));
  Test1Notify->JumpObserverRoot.AttachNewObserverList(Test1ListNode);

  if (!StateObserverMap.Contains(EState::Test1State))
  {
    StateObserverMap.Emplace(EState::Test1State, {});
  }
  StateObserverMap[EState::Test1State].Emplace(Test1ListNode);
  
  auto Test2ListNode = ARRanger::FObserverListRootNode::MakeListNode();
  Test2ListNode->Initialize();
  Test2ListNode->BindNewObserver(FSimpleDelegate::CreateStatic(&Private::RawTest2));
  Test2ListNode->BindNewObserver(FSimpleDelegate::CreateUObject(this, &AARTestObserverGameModeBase::OnGameModeBaseTest2));
  Test2Notify->JumpObserverRoot.AttachNewObserverList(Test2ListNode);
  
  if (!StateObserverMap.Contains(EState::Test2State))
  {
    StateObserverMap.Emplace(EState::Test2State, {});
  }
  StateObserverMap[EState::Test2State].Emplace(Test2ListNode);
  
  Handlers.Emplace(EState::Test1State, ::StaticCastSharedPtr<ARRanger::INotifyHandlerInterface>(Test1Notify));
  Handlers.Emplace(EState::Test2State, ::StaticCastSharedPtr<ARRanger::INotifyHandlerInterface>(Test2Notify));

  // Register all subjects
  {
    const UWorld* world = GetWorld();
    if (world != nullptr)
    {
      TArray<AActor*> subjects{};
      UGameplayStatics::GetAllActorsWithInterface(this, UObservableSubjectInterface::StaticClass(), subjects);

      if (subjects.Num() > 0)
      {
        for (AActor* subject : subjects)
        {
          IObservableSubjectInterface* subjectInterface = ::Cast<IObservableSubjectInterface>(subject);
          Subjects.AddUnique(subjectInterface);
        }
      }
    } 
  }

  SwitchToTest1State();
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