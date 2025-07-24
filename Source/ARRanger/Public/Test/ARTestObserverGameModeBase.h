// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"

#include "PlayerObservation/ObserverListNode.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "PlayerObservation/INotifyHandlerInterface.h"
#include "UObject/WeakInterfacePtr.h"

#include "ARTestObserverGameModeBase.generated.h" 


UCLASS()
class ARRANGER_API AARTestObserverGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

  private:
    enum EState
    {
      Test1State,
      Test2State,
    };
  
  public:
    AARTestObserverGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "TestObserver")
    void SwitchToTest1State();

    UFUNCTION(BlueprintCallable, Category = "TestObserver")
    void SwitchToTest2State();

  protected:
    virtual void StartPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  private:
    void OnGameModeBaseTest1();
    void OnGameModeBaseTest2();

  private:
    TMap< EState, TArray< TSharedPtr< ARRanger::FObserverListNode > > > StateObserverMap;
    
    TMap< EState, TSharedPtr< ARRanger::INotifyHandlerInterface > > Handlers;

    TArray< TWeakInterfacePtr < IObservableSubjectInterface > > Subjects;
	
};
