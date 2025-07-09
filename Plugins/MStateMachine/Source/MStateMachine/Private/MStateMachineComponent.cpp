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

// ���݂̃X�e�[�g��Ԃ��֐�
FGameplayTag FMStateHandle::GetStateTag() const
{
	return m_stateTag;
}

// �V�����X�e�[�g��ǉ�����֐�
FMStateHandle FMStateMachineStateList::AddEntry(TSubclassOf<UMStateDefinition> stateDef)
{
	check(stateDef != nullptr);
	check(OwnerComponent != nullptr);

	// �f�t�H���g�I�u�W�F�N�g���擾 
	// Class Default Object���g�����ƂŃe���v���[�g�I�ɒ��g���Q�Ƃł���
	const UMStateDefinition* mStateDefCDO = GetDefault<UMStateDefinition>(stateDef);

	// �����œn���ꂽ�X�e�[�g�����ɑ��݂��邩�`�F�b�N
	if (ContainsStateTag(mStateDefCDO->TransitionInfo.StateTag))
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Tag [%s] already exists"), *mStateDefCDO->TransitionInfo.ToString());
		return FMStateHandle{};
	}

	// �C���X�^���X(����)���L�q�����N���X���w�肷��
	TSubclassOf<UMStateInstance> instanceType = mStateDefCDO->InstanceType;
	if (instanceType == nullptr)
	{
		instanceType = UMStateInstance::StaticClass();
	}

	FMStateMachineStateListEntry newEntry;

	// �C���X�^���X�̎��̂��쐬
	UMStateInstance* stateInstance = NewObject<UMStateInstance>(OwnerComponent->GetOwner(), instanceType);

	// ���X�g��o�^
	newEntry.State = stateInstance;
	newEntry.StateDefinition = mStateDefCDO;
	Entries.Emplace(newEntry);

	// �X�e�[�g�n���h����Ԃ�
	// ��Ԃ�؂�ւ�����A�폜�����肷�邽�߂̎��ʎq
	return FMStateHandle(stateInstance, OwnerComponent, mStateDefCDO->TransitionInfo.StateTag);
}

// �w�肵���X�e�[�g���X�e�[�g���X�g����폜����֐�
void FMStateMachineStateList::RemoveEntry(FMStateHandle removeStateHandle)
{
	// ���݂��邩�ǂ����`�F�b�N
	if (!removeStateHandle.IsValid())
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("State Handle is invalid, Can not remove"));
		return;
	}

	// �^�O�ŃX�e�[�g��T���č폜����
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

// ���݂̃X�e�[�g����w�肵���^�O�̃X�e�[�g�ɐ؂�ւ����邩���f����֐�
UMStateInstance* FMStateMachineStateList::SwitchState(const UMStateInstance* currentStateInstance, FGameplayTag nextStateTag)
{
	// �X�e�[�g�^�O�����݂��邩�`�F�b�N
	UMStateInstance* nextState = nullptr;
	if (!nextStateTag.IsValid())
	{
		UE_LOG(LogMStateMachine, Error, TEXT("Next state Gameplay Tag Is Invalid"));
		return nextState;
	}

	// ���݂̃X�e�[�g����w�肳�ꂽ�X�e�[�g�ւ̑J�ڂ�������Ă��邩�`�F�b�N
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

	// �؂�ւ���X�e�[�g���Z�b�g
	nextState = GetStateByTag(nextStateTag);
	// �؂�ւ���X�e�[�g�����݂��邩�`�F�b�N
	if (nextState == nullptr)
	{
		UE_LOG(LogMStateMachine, Error, TEXT("State machine does not contains Gameplay Tag [%s]"), *nextStateTag.ToString());
	}

	// �؂�ւ���X�e�[�g��Ԃ�
	return nextState;
}

// �X�e�[�g�}�V���̏�ԃ��X�g���ɁA�w�肳�ꂽ FGameplayTag �����X�e�[�g�����łɑ��݂��邩�ǂ����𒲂ׂ�֐�
bool FMStateMachineStateList::ContainsStateTag(const FGameplayTag& tag) const
{
	bool isContain = false;

	// CreateConstIterator() �Ƃ� UE�ŕW���p�ӂ���Ă���C�e���[�^(���X�g�Ɣz��)���쐬����֐�
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

// �^�O����X�e�[�g���擾����֐�
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

// �X�e�[�g����^�O���擾����֐�
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

// UActorComponent �̏���
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

// �X�e�[�g�I������
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

// �X�e�[�g��Tick�������ғ�������֐�
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

// �X�e�[�g��Tick�������~�߂�֐�
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

// �����X�e�[�g���Z�b�g����֐�
void UMStateMachineComponent::SetEntryState(const FGameplayTag& EntryStateTag)
{
	if (m_bIsStateMachineStarted)
	{
		UE_LOG(LogMStateMachine, Warning, TEXT("Can not set entry state when state machine is started"));
		return;
	}

	m_currentState = m_stateList.GetStateByTag(EntryStateTag);
}

// �X�e�[�g���X�g�ɐV�����X�e�[�g��ǉ�����֐�
FMStateHandle UMStateMachineComponent::AddNewState(TSubclassOf<UMStateDefinition> StateDefClass)
{
	check(StateDefClass != nullptr)

	return m_stateList.AddEntry(StateDefClass);
	
}

// �����̃X�e�[�g���܂Ƃ߂ēo�^���Ă��ꂼ��̃n���h����Ԃ��֐�
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

// �X�e�[�g�����X�g����폜
void UMStateMachineComponent::RemoveState(FMStateHandle StateHandle)
{
	m_stateList.RemoveEntry(StateHandle);
}

// ���݂̃X�e�[�g����w�肵���^�O�̃X�e�[�g�ɐ؂�ւ���֐�
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

// �w�肵���X�e�[�g���X�e�[�g�}�V���ɑ��݂��邩����֐�
bool UMStateMachineComponent::ContainsStateTag(const FGameplayTag& Tag) const
{
	return m_stateList.ContainsStateTag(Tag);
}

// ���̐؂�ւ��X�e�[�g���؂�ւ����邩�Ԃ��֐�
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

// ���݂̃X�e�[�g�̃^�O���擾����֐�
FGameplayTag UMStateMachineComponent::GetCurrentStateTag() const
{
	if (m_currentState == nullptr)
	{
		return FGameplayTag::EmptyTag;
	}

	return m_stateList.GetTagByState(m_currentState);
}


