// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateMachineComponent.generated.h"

class UMStateInstance;
class UMStateDefinition;
class FMStateContext;

enum class EStateExitReason
{
  Transition,
  Uninitialize
};

USTRUCT(BlueprintType)
struct FMStateHandle
{
  GENERATED_BODY()
  
  friend class UMStateMachineComponent;

  MSTATEMACHINE_API FMStateHandle();


  MSTATEMACHINE_API FMStateHandle(
                UMStateInstance* state,
                UActorComponent* ownerComp,
                const FGameplayTag& stateTag
              );

  MSTATEMACHINE_API bool IsValid() const;
  MSTATEMACHINE_API FGameplayTag GetStateTag() const;
  
private:
  TWeakObjectPtr<UMStateInstance> m_state;
  TWeakObjectPtr<UActorComponent> m_ownerComp;
  FGameplayTag m_stateTag;
};

USTRUCT()
struct FMStateMachineStateListEntry
{
  GENERATED_BODY()

  friend struct FMStateMachineStateList;
  friend class UMStateMachineComponent;

  FMStateMachineStateListEntry()
    : State(nullptr)
    , StateDefinition(nullptr)
  {
  }

private:
  UPROPERTY()
  TObjectPtr<UMStateInstance> State;

  UPROPERTY()
  TObjectPtr<const UMStateDefinition> StateDefinition;

};

USTRUCT()
struct FMStateMachineStateList
{
  GENERATED_BODY()

public:
  FMStateMachineStateList();
  FMStateMachineStateList(UMStateMachineComponent* OwnerComp);

  FMStateHandle AddEntry(TSubclassOf<UMStateDefinition>);
  void RemoveEntry(FMStateHandle);

  UMStateInstance* SwitchState(const UMStateInstance* currentStateInstance, FGameplayTag);
  bool ContainsStateTag(const FGameplayTag&) const;
  UMStateInstance* GetStateByTag(const FGameplayTag&) const; 
  FGameplayTag GetTagByState(const UMStateInstance*) const;

private:

  friend class UMStateMachineComponent;

  UPROPERTY()
  TArray<FMStateMachineStateListEntry> Entries;

  UPROPERTY()
  TObjectPtr<UMStateMachineComponent> OwnerComponent;
};

struct FStateMachineInitializationParameters
{
  TObjectPtr<UObject> Owner;

  TObjectPtr<AController> OwnerController;
};








UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMStateMachineComponent : public UActorComponent
{
  GENERATED_BODY()

public:	
  MSTATEMACHINE_API UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:
  MSTATEMACHINE_API virtual void BeginPlay() override;

public:	

  //---UActorComponent Interface
  #pragma region UActorComponent Interface

  MSTATEMACHINE_API virtual void InitializeComponent() override;
  MSTATEMACHINE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  MSTATEMACHINE_API virtual void UninitializeComponent() override;

  #pragma endregion UActorComponent Interface
  //---End of UActorComponent Interface

  MSTATEMACHINE_API void Initialize(const FStateMachineInitializationParameters& Params);
  
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void StartTickState();

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void StopTickState();

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void SetEntryState(const FGameplayTag& EntryStateTag);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API TArray<FMStateHandle> AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void RemoveState(FMStateHandle StateHandle);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API bool SwitchNextState(const FGameplayTag& NextStateTag);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
  MSTATEMACHINE_API bool ContainsStateTag(const FGameplayTag& Tag) const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API FGameplayTag GetCurrentStateTag() const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API FGameplayTag GetStateTagByInstance(const UMStateInstance* StateInstance) const;

  MSTATEMACHINE_API FMStateContext* GetContext() const { return m_context.Get(); }

private:
  void EnterStateInternal(const UMStateInstance* PreviousStateInstance, UMStateInstance* NextStateInstance);
  void TickStateInternal(UMStateInstance* CurrentStateInstance, float DeltaTime);
  void ExitStateInternal(UMStateInstance* StateInstance, const UMStateInstance* NextStateInstance, const EStateExitReason Reason = EStateExitReason::Transition);

private:
  UPROPERTY()
  FMStateMachineStateList m_stateList;

  UPROPERTY()
  TObjectPtr<UMStateInstance> m_currentState;

  UPROPERTY(EditAnywhere)
  bool bAutoInitializeContext;

  TSharedPtr<FMStateContext, ESPMode::NotThreadSafe> m_context;

  uint8 m_bIsStateMachineStarted : 1;
  uint8 m_bCanTickStateMachine : 1;
    
};
