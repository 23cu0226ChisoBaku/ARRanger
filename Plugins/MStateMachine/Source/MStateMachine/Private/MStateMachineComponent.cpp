// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateMachineComponent.h"

#include "MStateInstance.h"
#include "MStateDefinition.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateMachineComponent)

bool FMStateHandle::IsValid() const
{
  return m_state.IsValid() && m_ownerComp.IsValid() && m_stateTag.IsValid();
}

// 現在のステートを返す関数
FGameplayTag FMStateHandle::GetStateTag() const
{
	return m_stateTag;
}

// 新しいステートを追加する関数
FMStateHandle FMStateMachineStateList::AddEntry(TSubclassOf<UMStateDefinition> stateDef)
{
	check(stateDef != nullptr);
	check(OwnerComponent != nullptr);

	// デフォルトオブジェクトを取得 
	// Class Default Objectを使うことでテンプレート的に中身を参照できる
	const UMStateDefinition* mStateDefCDO = GetDefault<UMStateDefinition>(stateDef);

	// 引数で渡されたステートが既に存在するかチェック
	if (ContainsStateTag(mStateDefCDO->TransitionInfo.StateTag))
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Tag [%s] already exists"), *mStateDefCDO->TransitionInfo.ToString());
		return FMStateHandle{};
	}

	// インスタンス(挙動)を記述したクラスを指定する
	TSubclassOf<UMStateInstance> instanceType = mStateDefCDO->InstanceType;
	if (instanceType == nullptr)
	{
		instanceType = UMStateInstance::StaticClass();
	}

	FMStateMachineStateListEntry newEntry;

	// インスタンスの実体を作成
	UMStateInstance* stateInstance = NewObject<UMStateInstance>(OwnerComponent->GetOwner(), instanceType);

	// リストを登録
	newEntry.State = stateInstance;
	newEntry.StateDefinition = mStateDefCDO;
	Entries.Emplace(newEntry);

	// ステートハンドルを返す
	// 状態を切り替えたり、削除したりするための識別子
	return FMStateHandle(stateInstance, OwnerComponent, mStateDefCDO->TransitionInfo.StateTag);
}

// 指定したステートをステートリストから削除する関数
void FMStateMachineStateList::RemoveEntry(FMStateHandle removeStateHandle)
{
	// 存在するかどうかチェック
	if (!removeStateHandle.IsValid())
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Handle is invalid, Can not remove"));
		return;
	}

	// タグでステートを探して削除する
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

// 現在のステートから指定したタグのステートに切り替えられるか判断する関数
UMStateInstance* FMStateMachineStateList::SwitchState(const UMStateInstance* currentStateInstance, FGameplayTag nextStateTag)
{
	// ステートタグが存在するかチェック
	UMStateInstance* nextState = nullptr;
	if (!nextStateTag.IsValid())
	{
		UE_LOG(LogMStateMachine, Error, TEXT("Next state Gameplay Tag Is Invalid"));
		return nextState;
	}

	// 現在のステートから指定されたステートへの遷移が許可されているかチェック
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

	// 切り替えるステートをセット
	nextState = GetStateByTag(nextStateTag);
	// 切り替えるステートが存在するかチェック
	if (nextState == nullptr)
	{
		UE_LOG(LogMStateMachine, Error, TEXT("State machine does not contains Gameplay Tag [%s]"), *nextStateTag.ToString());
	}

	// 切り替えるステートを返す
	return nextState;
}

// ステートマシンの状態リスト内に、指定された FGameplayTag を持つステートがすでに存在するかどうかを調べる関数
bool FMStateMachineStateList::ContainsStateTag(const FGameplayTag& tag) const
{
	bool isContain = false;

	// CreateConstIterator() とは UEで標準用意されているイテレータ(リストと配列)を作成する関数
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

// タグからステートを取得する関数
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

// ステートからタグを取得する関数
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

// UActorComponent の処理
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

// ステート終了処理
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

// ステートのTick処理を稼働させる関数
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

// ステートのTick処理を止める関数
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

// 初期ステートをセットする関数
void UMStateMachineComponent::SetEntryState(const FGameplayTag& EntryStateTag)
{
	if (m_bIsStateMachineStarted)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Can not set entry state when state machine is started"));
		return;
	}

	m_currentState = m_stateList.GetStateByTag(EntryStateTag);
}

// ステートリストに新しいステートを追加する関数
FMStateHandle UMStateMachineComponent::AddNewState(TSubclassOf<UMStateDefinition> StateDefClass)
{
	check(StateDefClass != nullptr)

	return m_stateList.AddEntry(StateDefClass);
	
}

// 複数のステートをまとめて登録してそれぞれのハンドルを返す関数
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

// ステートをリストから削除
void UMStateMachineComponent::RemoveState(FMStateHandle StateHandle)
{
	m_stateList.RemoveEntry(StateHandle);
}

// 現在のステートから指定したタグのステートに切り替える関数
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

// 指定したステートがステートマシンに存在するか見る関数
bool UMStateMachineComponent::ContainsStateTag(const FGameplayTag& Tag) const
{
	return m_stateList.ContainsStateTag(Tag);
}

// 次の切り替えステートが切り替えられるか返す関数
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

// 現在のステートのタグを取得する関数
FGameplayTag UMStateMachineComponent::GetCurrentStateTag() const
{
	if (m_currentState == nullptr)
	{
		return FGameplayTag::EmptyTag;
	}

	return m_stateList.GetTagByState(m_currentState);
}


