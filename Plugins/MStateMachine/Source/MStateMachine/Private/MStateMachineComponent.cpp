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

FMStateHandle::FMStateHandle(const UMStateInstance* State, const UActorComponent* OwnerComp, const FGameplayTag& StateTag)
  : m_state(State)
  , m_ownerComp(OwnerComp)
  , m_stateTag(StateTag)
{ }

bool FMStateHandle::IsValid() const
{
  return m_state.IsValid() && m_ownerComp.IsValid() && m_stateTag.IsValid();
}

FGameplayTag FMStateHandle::GetStateTag() const
{
	return m_stateTag;
}

FMStateMachineStateListEntry::FMStateMachineStateListEntry()
  : State{nullptr}
  , StateDefinition{nullptr}
{
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

  // デフォルトオブジェクトでエントリーオブジェクトを初期化する
	const UMStateDefinition* mStateDefCDO = GetDefault<UMStateDefinition>(stateDef);
  const FGameplayTag& stateDefTag = mStateDefCDO->TagInfo.StateTag;
	if (ContainsStateTag(stateDefTag))
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Tag [%s] already exists"), *mStateDefCDO->TagInfo.ToString());
		return FMStateHandle{};
	}

	TSubclassOf<UMStateInstance> instanceType = mStateDefCDO->InstanceType;
	if (instanceType == nullptr)
	{
    // デフォルト値にFallback
		instanceType = UMStateInstance::StaticClass();
	}

	UMStateInstance* stateInstance = NewObject<UMStateInstance>(OwnerComponent->GetOwner(), instanceType);
  
  // 生成したステートを初期化する
  FStateInitializationParameters initParams;
  initParams.OwnerActor = OwnerComponent->GetOwner();
  initParams.OwnerStateMachineComponent = OwnerComponent;
  stateInstance->InitializeState(initParams);
  
	FMStateMachineStateListEntry newEntry;
	newEntry.State = stateInstance;
	newEntry.StateDefinition = mStateDefCDO;

	Entries.Emplace(newEntry);

	return FMStateHandle{stateInstance, OwnerComponent, stateDefTag};
}

void FMStateMachineStateList::RemoveEntry(const FMStateHandle& RemoveStateHandle)
{
	if (!RemoveStateHandle.IsValid())
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Handle is invalid, Can not remove"));
		return;
	}

  // イテレーターで走査し、見つけたらリストから削除する
  using EntryIterator = decltype(Entries)::TIterator;
	for (EntryIterator entryIt = Entries.CreateIterator(); entryIt; ++entryIt)
	{
		const FMStateMachineStateListEntry& entry = *entryIt;
		if (entry.StateDefinition->TagInfo.StateTag == RemoveStateHandle.GetStateTag())
		{
      // ステート初期化解除
      FStateUninitializationParameters params;
      entryIt->State->UninitializeState(params);
			entryIt.RemoveCurrent();
			break;
		}
	}
}

UMStateInstance* FMStateMachineStateList::TrySwitchState(const UMStateInstance* CurrentStateInstance, const FGameplayTag& NextStateTag)
{
  if (!NextStateTag.IsValid())
	{
    UE_LOG(LogMStateMachine, Error, TEXT("Next state FGameplayTag is invalid"));
		return nullptr;
	}

  auto searchInstancePredicate = [&CurrentStateInstance](const FMStateMachineStateListEntry& Entry)
  {
    return Entry.State == CurrentStateInstance;
  };
  
  FMStateMachineStateListEntry* foundEntry = Entries.FindByPredicate(searchInstancePredicate);
  // 現在のステートがリストに存在しない場合
  if (foundEntry == nullptr)
  {
    UE_LOG(LogMStateMachine, Error, TEXT("Input state instance is invalid or not exist"));
    return nullptr;
  }

  // 現在のステートが次のステートに切り替えられない場合
  if (!foundEntry->StateDefinition->TagInfo.NextTransitionTags.Contains(NextStateTag))
  {
    UE_LOG(LogMStateMachine, Error, TEXT("Can not switch to next state [%s]"), *NextStateTag.ToString());
    return nullptr;
  }
	
  
  // 切り替えられるので次のステートを取得
  UMStateInstance* nextState = GetStateByTag(NextStateTag);
	if (nextState == nullptr)
	{
		UE_LOG(LogMStateMachine, Error, TEXT("State machine:[%s] does not contains FGameplayTag [%s]"), *::GetNameSafe(OwnerComponent), *NextStateTag.ToString());
	}

	return nextState;
}

bool FMStateMachineStateList::ContainsStateTag(const FGameplayTag& StateTag) const
{
  return GetStateByTag(StateTag) != nullptr;
}

UMStateInstance* FMStateMachineStateList::GetStateByTag(const FGameplayTag& StateTag) const
{
  auto searchTagPredicate = [&StateTag](const FMStateMachineStateListEntry& Entry)
  {
    return Entry.StateDefinition->TagInfo.StateTag == StateTag;
  };

  const FMStateMachineStateListEntry* foundEntry = Entries.FindByPredicate(searchTagPredicate);
  return (foundEntry != nullptr) ? foundEntry->State : nullptr;
}

FGameplayTag FMStateMachineStateList::GetTagByState(const UMStateInstance* StateInstance) const
{
  auto searchInstancePredicate = [&StateInstance](const FMStateMachineStateListEntry& Entry)
  {
    return Entry.State == StateInstance;
  };

  const FMStateMachineStateListEntry* foundEntry = Entries.FindByPredicate(searchInstancePredicate);
  return (foundEntry != nullptr) ? foundEntry->StateDefinition->TagInfo.StateTag : FGameplayTag::EmptyTag;
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

    // 自動的にコンテキストを作成
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

void UMStateMachineComponent::InitializeComponent()
{
	Super::InitializeComponent();
}


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
  // 現在のステートを終了させる
  ExitStateInternal(m_currentState, nullptr, EStateExitReason::Uninitialize);
  m_currentState = nullptr;

  for (const FMStateMachineStateListEntry& entry : m_stateList.Entries)
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

  // コンテキストを初期化する
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

  if (m_currentState == nullptr)
  {
    UE_LOG(LogMStateMachine, Error, TEXT("Tag is not exists in this state machine"));
  }
}

FMStateHandle UMStateMachineComponent::AddNewState(TSubclassOf<UMStateDefinition> StateDefClass)
{
	check(StateDefClass != nullptr)

	return m_stateList.AddEntry(StateDefClass);
}

void UMStateMachineComponent::AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses, TArray<FMStateHandle>& OutHandles)
{
  OutHandles.Reset(StateDefClasses.Num());

	for (const TSubclassOf<UMStateDefinition>& stateDefClass : StateDefClasses)
	{
		check(stateDefClass != nullptr);

    OutHandles.Emplace(m_stateList.AddEntry(stateDefClass));	
	}
}

void UMStateMachineComponent::RemoveState(const FMStateHandle& StateHandle)
{
	m_stateList.RemoveEntry(StateHandle);
}

bool UMStateMachineComponent::SwitchNextState(const FGameplayTag& NextStateTag)
{
	UMStateInstance* nextState = m_stateList.TrySwitchState(m_currentState, NextStateTag);
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
  // 現在のステートかつ次のステートに切り替えられるかを確認するラムダ式
  auto searchCurrentNextTransitionTagPredicate = [&](const FMStateMachineStateListEntry& Entry)
  {
    return (Entry.State == m_currentState) && Entry.StateDefinition->TagInfo.NextTransitionTags.Contains(NextStateTag);
  };

  const FMStateMachineStateListEntry* foundEntry = m_stateList.Entries.FindByPredicate(searchCurrentNextTransitionTagPredicate);
  return foundEntry != nullptr;
}

FGameplayTag UMStateMachineComponent::GetCurrentStateTag() const
{
	return GetStateTagByInstance(m_currentState);
}

FGameplayTag UMStateMachineComponent::GetStateTagByInstance(const UMStateInstance* StateInstance) const
{
  return m_stateList.GetTagByState(StateInstance);
}

int32 UMStateMachineComponent::GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const
{
  OutTags.Reset();

  for(const auto& entry : m_stateList.Entries)
  {
    if (entry.State == m_currentState)
    {
      const TSet<FGameplayTag>& transitionTags = entry.StateDefinition->TagInfo.NextTransitionTags;
      OutTags.Reset(transitionTags.Num());
      
      for (const FGameplayTag& tag : transitionTags)
      {
        OutTags.AddUnique(tag);
      }
      break;
    }
  }

  return OutTags.Num();
}

void UMStateMachineComponent::EnterStateInternal(const UMStateInstance* PreviousStateInstance, UMStateInstance* NextStateInstance)
{
  check((NextStateInstance != nullptr));
  
  FStateTransitionParameters params;
  params.TransitionTag = GetStateTagByInstance(PreviousStateInstance);
  params.Transition = EStateTransitionType::Enter;
  params.Context = m_context;
  NextStateInstance->EnterState(params);
}

void UMStateMachineComponent::TickStateInternal(UMStateInstance* StateInstance, float DeltaTime)
{
  check(StateInstance != nullptr);

  FStateTickParameters params;
  params.DeltaTime = DeltaTime;
  params.Context = m_context;
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

