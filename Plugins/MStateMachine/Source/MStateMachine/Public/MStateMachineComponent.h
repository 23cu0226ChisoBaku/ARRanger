// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateMachineComponent.generated.h"

class UMStateInstance;
class UMStateDefinition;

USTRUCT(BlueprintType)
struct FMStateHandle
{
	GENERATED_BODY()
	
	friend class UMStateMachineComponent;

	FMStateHandle()
		: m_state(nullptr)
		, m_ownerComp(nullptr)
		, m_stateTag(FGameplayTag::EmptyTag)
	{
	}

	FMStateHandle(
								UMStateInstance* state,
								UActorComponent* ownerComp,
								const FGameplayTag& stateTag
							 )
		: m_state(state)
		, m_ownerComp(ownerComp)
		, m_stateTag(stateTag)
	{
	}

	bool IsValid() const;
	FGameplayTag GetStateTag() const;
	
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
	FMStateMachineStateList()
		: Entries{}
		, OwnerComponent(nullptr)
	{
	}
	FMStateMachineStateList(UActorComponent* ownerComp)
		: Entries{}
		, OwnerComponent(ownerComp)
	{
	}

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
	TObjectPtr<UActorComponent> OwnerComponent;
};










UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MSTATEMACHINE_API UMStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:	

	//---UActorComponent Interface
	#pragma region UActorComponent Interface

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UninitializeComponent() override;

	#pragma endregion UActorComponent Interface
	//---End of UActorComponent Interface

	
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StartTickState();

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StopTickState();

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void SetEntryState(const FGameplayTag& EntryStateTag);

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	TArray<FMStateHandle> AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses);

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void RemoveState(FMStateHandle StateHandle);

	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	bool SwitchNextState(const FGameplayTag& NextStateTag);

	UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
	bool ContainsStateTag(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	FGameplayTag GetCurrentStateTag() const;

private:
	UPROPERTY()
	FMStateMachineStateList m_stateList;

	UPROPERTY()
	TObjectPtr<UMStateInstance> m_currentState;

	uint8 m_bIsStateMachineStarted : 1;
	uint8 m_bCanTickStateMachine : 1;
		
};
