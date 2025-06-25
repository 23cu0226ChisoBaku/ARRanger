// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateMachineComponent.h"

#include "MStateInstance.h"
#include "MStateDefinition.h"
#include "MStateMachineLogChannels.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateMachineComponent)

bool FMStateHandle::IsValid() const
{
  return m_state.IsValid() && m_ownerComp.IsValid() && m_stateTag.IsValid();
}

FGameplayTag FMStateHandle::GetStateTag() const
{
	return m_stateTag;
}

FMStateHandle FMStateMachineStateList::AddEntry(TSubclassOf<UMStateDefinition> stateDef)
{
	check(stateDef != nullptr);
	check(OwnerComponent != nullptr);

	const UMStateDefinition* mStateDefCDO = GetDefault<UMStateDefinition>(stateDef);
	if (ContainsStateTag(mStateDefCDO->TransitionInfo.StateTag))
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Tag [%s] already exists"), *mStateDefCDO->TransitionInfo.ToString());
		return FMStateHandle{};
	}

	TSubclassOf<UMStateInstance> instanceType = mStateDefCDO->InstanceType;
	if (instanceType == nullptr)
	{
		instanceType = UMStateInstance::StaticClass();
	}

	FMStateMachineStateListEntry newEntry;

	UMStateInstance* stateInstance = NewObject<UMStateInstance>(OwnerComponent->GetOwner(), instanceType);
	newEntry.State = stateInstance;
	newEntry.StateDefinition = mStateDefCDO;

	Entries.Emplace(newEntry);

	return FMStateHandle(stateInstance, OwnerComponent, mStateDefCDO->TransitionInfo.StateTag);
}

void FMStateMachineStateList::RemoveEntry(FMStateHandle removeStateHandle)
{
	if (!removeStateHandle.IsValid())
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Handle is invalid, Can not remove"));
		return;
	}

	for (auto entryIt = Entries.CreateIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.StateDefinition->TransitionInfo.StateTag == removeStateHandle.GetStateTag())
		{
			entryIt.RemoveCurrent();
			break;
		}
	}

}

UMStateInstance* FMStateMachineStateList::SwitchState(const UMStateInstance* currentStateInstance, FGameplayTag nextStateTag)
{
	UMStateInstance* nextState = nullptr;
	if (!nextStateTag.IsValid())
	{
		UE_LOG(LogMStateMachine, Error, TEXT("Next state Gameplay Tag Is Invalid"));
		return nextState;
	}

	FGameplayTag currentStateTag = GetTagByState(currentStateInstance);
	if (currentStateTag.IsValid())
	{
		for (auto entryIt = Entries.CreateIterator(); entryIt; ++entryIt)
		{
			const FMStateMachineStateListEntry& entry = *entryIt;
			if (entry.StateDefinition->TransitionInfo.StateTag == currentStateTag)
			{
				if (!entry.StateDefinition->TransitionInfo.NextTransitionTags.Contains(nextStateTag))
				{
					UE_LOG(LogMStateMachine, Error, TEXT("Can not Switch to next State [%s]"), *nextStateTag.ToString());
					return nextState;
				}	
			}
		}
	}

	nextState = GetStateByTag(nextStateTag);
	if (nextState == nullptr)
	{
		UE_LOG(LogMStateMachine, Error, TEXT("State machine does not contains Gameplay Tag [%s]"), *nextStateTag.ToString());
	}

	return nextState;

}

bool FMStateMachineStateList::ContainsStateTag(const FGameplayTag& tag) const
{
	bool isContain = false;

	for (auto entryIt = Entries.CreateConstIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.StateDefinition->TransitionInfo.StateTag == tag)
		{
			isContain = true;
			break;
		}
	}

	return isContain;
}

UMStateInstance* FMStateMachineStateList::GetStateByTag(const FGameplayTag& tag) const
{
	UMStateInstance* foundState = nullptr;

	for (auto entryIt = Entries.CreateConstIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.StateDefinition->TransitionInfo.StateTag == tag)
		{
			foundState = entry.State;
			break;
		}
	}

	return foundState;
}

FGameplayTag FMStateMachineStateList::GetTagByState(const UMStateInstance* stateInstance) const
{
	FGameplayTag foundTag = FGameplayTag::EmptyTag;

	for (auto entryIt = Entries.CreateConstIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.State == stateInstance)
		{
			foundTag = entry.StateDefinition->TransitionInfo.StateTag;
			break;
		}
	}

	return foundTag;
}

UMStateMachineComponent::UMStateMachineComponent(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
	, m_stateList(this)
	, m_bIsStateMachineStarted(false)
	, m_bCanTickStateMachine(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UMStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMStateMachineComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
// Called every frame
void UMStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bCanTickStateMachine)
	{
		if (m_currentState != nullptr)
		{
			m_currentState->TickState(DeltaTime);
		}
	}

}

void UMStateMachineComponent::UninitializeComponent()
{
	if (m_currentState != nullptr)
	{
		m_currentState->ExitState();
		m_currentState = nullptr;
	}

	m_stateList.Entries.Empty();

	m_bCanTickStateMachine = false;
	m_bIsStateMachineStarted = false;

	Super::UninitializeComponent();
}

void UMStateMachineComponent::StartTickState()
{
	if (m_bCanTickStateMachine)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Can not start when state machine is running"));
		return;
	}

	if (m_currentState == nullptr)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Call SetEntryState() before start"));
		return;
	}

	SetComponentTickEnabled(true);
	m_bCanTickStateMachine = true;

	m_bIsStateMachineStarted = true;
}

void UMStateMachineComponent::StopTickState()
{
	if (!m_bCanTickStateMachine || !m_bIsStateMachineStarted)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Can not stop when state machine is not running"));
		return;
	}

	SetComponentTickEnabled(false);
	m_bCanTickStateMachine = false;
}

void UMStateMachineComponent::SetEntryState(const FGameplayTag& EntryStateTag)
{
	if (m_bIsStateMachineStarted)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Can not set entry state when state machine is started"));
		return;
	}

	m_currentState = m_stateList.GetStateByTag(EntryStateTag);
}

FMStateHandle UMStateMachineComponent::AddNewState(TSubclassOf<UMStateDefinition> StateDefClass)
{
	check(StateDefClass != nullptr)

	return m_stateList.AddEntry(StateDefClass);
	
}

TArray<FMStateHandle> UMStateMachineComponent::AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses)
{
	TArray<FMStateHandle> handles{};
	for (const TSubclassOf<UMStateDefinition>& stateDefClass : StateDefClasses)
	{
		check(stateDefClass != nullptr);
		FMStateHandle handle = m_stateList.AddEntry(stateDefClass);

		if (handle.IsValid())
		{
			handles.Emplace(handle);
		}
	}

	return handles;
}

void UMStateMachineComponent::RemoveState(FMStateHandle StateHandle)
{
	m_stateList.RemoveEntry(StateHandle);
}

bool UMStateMachineComponent::SwitchNextState(const FGameplayTag& NextStateTag)
{
	UMStateInstance* nextState = m_stateList.SwitchState(m_currentState, NextStateTag);
	if (nextState == nullptr || m_currentState == nextState)
	{
		return false;
	}

	if (m_currentState != nullptr)
	{
		m_currentState->ExitState();
	}

	m_currentState = nextState;
	m_currentState->EntryState();

	return true;
}

bool UMStateMachineComponent::ContainsStateTag(const FGameplayTag& Tag) const
{
	return m_stateList.ContainsStateTag(Tag);
}

bool UMStateMachineComponent::CanSwitchToNext(const FGameplayTag& NextStateTag) const
{
	bool bCanSwitch = false;
	for (auto entryIt = m_stateList.Entries.CreateConstIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.State == m_currentState)
		{
			bCanSwitch = entry.StateDefinition->TransitionInfo.NextTransitionTags.Contains(NextStateTag);
			break;
		}
	}
	
	return bCanSwitch;
}

FGameplayTag UMStateMachineComponent::GetCurrentStateTag() const
{
	if (m_currentState == nullptr)
	{
		return FGameplayTag::EmptyTag;
	}

	return m_stateList.GetTagByState(m_currentState);
}


