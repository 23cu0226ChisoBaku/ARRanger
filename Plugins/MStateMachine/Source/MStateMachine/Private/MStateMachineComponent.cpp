// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateMachineComponent.h"

#include "MStateInstance.h"
#include "MStateDefinition.h"
#include "MStateMachineLogChannels.h"
#include "GameplayTagContainer.h"
#include "MStateContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateMachineComponent)

FMStateHandle::FMStateHandle()
  : m_state(nullptr)
  , m_ownerComp(nullptr)
  , m_stateTag(FGameplayTag::EmptyTag)
{ }

FMStateHandle::FMStateHandle(
              UMStateInstance* state,
              UActorComponent* ownerComp,
              const FGameplayTag& stateTag
              )
  : m_state(state)
  , m_ownerComp(ownerComp)
  , m_stateTag(stateTag)
{ }

bool FMStateHandle::IsValid() const
{
  return m_state.IsValid() && m_ownerComp.IsValid() && m_stateTag.IsValid();
}

FGameplayTag FMStateHandle::GetStateTag() const
{
	return m_stateTag;
}

FMStateMachineStateList::FMStateMachineStateList()
  : Entries{}
  , OwnerComponent{nullptr}
{ }

FMStateMachineStateList::FMStateMachineStateList(UMStateMachineComponent* OwnerComp)
  : Entries{}
  , OwnerComponent{OwnerComp}
{ 
  check(OwnerComponent != nullptr);
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

  FStateInitializationParameters initParams;
  initParams.OwnerActor = OwnerComponent->GetOwner();
  initParams.OwnerStateMachineComponent = OwnerComponent;
  stateInstance->InitializeState(initParams);

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
      FStateUninitializationParameters params;
      entryIt->State->UninitializeState(params);
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
  if (stateInstance == nullptr)
  {
    return FGameplayTag::EmptyTag;
  }
 
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

UMStateMachineComponent::UMStateMachineComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_stateList{this}
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

  if (bAutoInitializeContext)
  {
    APawn* ownerPawn = ::Cast<APawn>(GetOwner());
    if (ownerPawn != nullptr)
    {
      FStateMachineInitializationParameters params;

      params.Owner = ::StaticCast<UObject*>(ownerPawn);
      params.OwnerController = ownerPawn->GetController();

      Initialize(params);
    }

    if (m_context == nullptr)
    {
      UE_LOG(LogMStateMachine, Error, TEXT("Failed to initialize context automatically because owner:{%s} of [%s] is not a pawn.\n Please call Initialize MANUALLY!!!"), *GetNameSafe(GetOwner()), *GetNameSafe(this));
    }
  }
}
// Called every frame
void UMStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bCanTickStateMachine)
	{
    TickStateInternal(m_currentState, DeltaTime);
	}
}

void UMStateMachineComponent::UninitializeComponent()
{
  ExitStateInternal(m_currentState, nullptr, EStateExitReason::Uninitialize);
  m_currentState = nullptr;

  for (auto& entry : m_stateList.Entries)
  {
    if (entry.State != nullptr)
    {
      FStateUninitializationParameters params;
      entry.State->UninitializeState(params);
    }
  }

	m_stateList.Entries.Empty();

	Super::UninitializeComponent();
}

void UMStateMachineComponent::Initialize(const FStateMachineInitializationParameters& Params)
{
  if (m_context != nullptr)
  {
    return;
  }

  m_context = ::MakeShared<FMStateContext, ESPMode::NotThreadSafe>();

  FMStateContextInitializeParameters ctxInitParams;
  ctxInitParams.Owner = Params.Owner;
  ctxInitParams.OwnerController = Params.OwnerController;
  ctxInitParams.StateMachineComponent = this;

  m_context->InitializeContext(ctxInitParams);
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
  handles.Reset(StateDefClasses.Num());

	for (const TSubclassOf<UMStateDefinition>& stateDefClass : StateDefClasses)
	{
		check(stateDefClass != nullptr);

    handles.Emplace(m_stateList.AddEntry(stateDefClass));	
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
	if ((nextState == nullptr) || (m_currentState == nextState))
	{
		return false;
	}

  // Exit current state
  ExitStateInternal(m_currentState, nextState);
	
  // Enter next state
  EnterStateInternal(m_currentState, nextState);

  m_currentState = nextState;

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
	return GetStateTagByInstance(m_currentState);
}

FGameplayTag UMStateMachineComponent::GetStateTagByInstance(const UMStateInstance* StateInstance) const
{
  return m_stateList.GetTagByState(StateInstance);
}

void UMStateMachineComponent::EnterStateInternal(const UMStateInstance* PreviousStateInstance, UMStateInstance* NextStateInstance)
{
  check((NextStateInstance != nullptr));
  
  FStateTransitionParameters params;
  params.TransitionTag = GetStateTagByInstance(PreviousStateInstance);
  params.Transition = EStateTransitionType::Enter;
  params.Context = m_context;

  // First run c++ implementation then run blueprint implementation
  NextStateInstance->EnterState(params);
}

void UMStateMachineComponent::TickStateInternal(UMStateInstance* StateInstance, float DeltaTime)
{
  check(StateInstance != nullptr);

  FStateTickParameters params;
  params.DeltaTime = DeltaTime;
  params.Context = m_context;

  // First run c++ implementation then run blueprint implementation
  StateInstance->TickState(params);
}

void UMStateMachineComponent::ExitStateInternal(UMStateInstance* PreviousStateInstance, const UMStateInstance* NextStateInstance, const EStateExitReason Reason)
{
  if (PreviousStateInstance == nullptr)
  {
    return;
  }

  FStateTransitionParameters params;

  using enum EStateExitReason;
  switch (Reason)
  {
    case Transition:
    {
      params.TransitionTag = GetStateTagByInstance(NextStateInstance);
    }
    break;
    
    case Uninitialize:
    default:
    {
      params.TransitionTag = FGameplayTag::EmptyTag;
    }
    break;
  }  

  params.Transition = EStateTransitionType::Exit;
  params.Context = m_context;

  PreviousStateInstance->ExitState(params);
}

