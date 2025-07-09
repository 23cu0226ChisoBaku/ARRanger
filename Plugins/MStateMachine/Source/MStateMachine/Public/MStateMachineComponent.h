// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UActorComponent ���g�p���邽�߂̃C���N���[�h
// UE �̃R���|�[�l���g�ׁ[�X�݌v�̊�{�N���X
// �A�N�^�[�ɋ@�\��ǉ�����u�R���|�[�l���g�v���`�E�g�p���邽��
#include "Components/ActorComponent.h"

// GamePlay Tags ���g�����߂̃C���N���[�h
// �^�O���g���ď�Ԃ�J�e�S�����_��ɊǗ��ł���
// ��Ԃ⑮����\���u�Q�[���v���C�^�O�v���g�p���邽��
#include "GameplayTagContainer.h"

// LogMState �� LogMStateMachine �Ƃ������O�J�e�S�����́i��`�j���s���Ă���
#include "MStateMachineLogChannels.h"

// ����ȃv���v���Z�b�T�f�B���N�e�B�u
// Unreal Engine �� ���t���N�V�����@�\�iUCLASS/USTRUCT/UENUM �Ȃǁj�𐳂����g������
// UCLASS(), USTRUCT(), UENUM() �Ȃǂ̃}�N�����g���Ă���ꍇ�A���� .generated.h ��K�� �N���X�錾�̒��O�ɃC���N���[�h����K�v������
#include "MStateMachineComponent.generated.h"

class UMStateInstance;
class UMStateDefinition;

// FMStateHandle �Ƃ��� �\���̂��u���[�v�����g����g����i�ϐ��Ƃ��Ď��Ă�悤�ɂ���j
USTRUCT(BlueprintType)
struct FMStateHandle
{
	GENERATED_BODY()
	
	friend class UMStateMachineComponent;

	// FMStateHandle �\���̂� �f�t�H���g�R���X�g���N�^ �̒�`
	// �����l�𖾎��I�ɐݒ肷��R���X�g���N�^
	
	// �R���X�g���N�^
	// nullptr �ɂ��Ă������ƂŁA���������̃|�C���^�Q�Ƃɂ��N���b�V���i��ǃ|�C���^�j��h��
	// FGameplayTag::EmptyTag ���g�����ƂŁu���̃n���h���ɂ͂܂��^�O���ݒ肳��Ă��Ȃ��v���Ƃ𖾎��ł���B
	// �������𖾊m�ɂ��邱�ƂŁA��� IsValid() �Ȃǂ̃`�F�b�N�����₷���Ȃ�B
	FMStateHandle()
		: m_state(nullptr)						
		, m_ownerComp(nullptr)
		, m_stateTag(FGameplayTag::EmptyTag)
	{
	}

	// �R���X�g���N�^
	// �w�肳�ꂽ�l�Ńn���h���\���̂�������
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

	// �L���ł��邩�m�F����֐�
	bool IsValid() const;
	// �X�e�[�g�̃^�O���擾����֐�
	// FGameplayTag��UE�ŕW���p�ӂ���Ă���\����
	FGameplayTag GetStateTag() const;
	
private:
	// TWeakObjectPtr �Ƃ�
	// ���ł̉\�������� UObject �����S�ɎQ�Ƃ������Ƃ��Ɏg���X�}�[�g�|�C���^
	// GC �ō폜���ꂽ�����`�F�b�N�ł���E�I�u�W�F�N�g���܂������Ă���΃A�N�Z�X�ł���
	TWeakObjectPtr<UMStateInstance> m_state;		// �X�e�[�g�ۊǗp
	TWeakObjectPtr<UActorComponent> m_ownerComp;	// �I�[�i�[�R���|�l���g�ۊǗp
	FGameplayTag m_stateTag;						// �X�e�[�g�̃^�O
};


// ���̍\���̂� Unreal �̃��t���N�V�����V�X�e���ɓo�^����
// �i�V���A���C�Y�E�G�f�B�^�ŎQ�Ƃł���j
USTRUCT()
struct FMStateMachineStateListEntry
{
	GENERATED_BODY()

	// �X�e�[�g�}�V��������Ԉꗗ
	friend struct FMStateMachineStateList;
	friend class UMStateMachineComponent;

	FMStateMachineStateListEntry()
		: State(nullptr)
		, StateDefinition(nullptr)	// �X�e�[�g�̒�`
	{
	}

private:
	// TObject = GC �Ώ�
	// TObjectPtr�FCG�ɑ΂��Ĉ��S�Ȑ錾
	UPROPERTY()
	TObjectPtr<UMStateInstance> State;

	UPROPERTY()
	TObjectPtr<const UMStateDefinition> StateDefinition;
};

// �X�e�[�g�̏�Ԃ��Ǘ�����
USTRUCT()
struct FMStateMachineStateList
{
	GENERATED_BODY()

	// �X�e�[�g
public:
	// �R���X�g���N�^
	FMStateMachineStateList()
		: Entries{}
		, OwnerComponent(nullptr)
	{
	}

	// �R���X�g���N�^
	FMStateMachineStateList(UActorComponent* ownerComp)
		: Entries{}
		, OwnerComponent(ownerComp)
	{
	}

	// �֐��錾	
	FMStateHandle AddEntry(TSubclassOf<UMStateDefinition>);
	void RemoveEntry(FMStateHandle);

	// �֐��錾
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






// ��ԑJ�ڃX�e�[�g�}�V�����Ǘ����� ActorComponent 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MSTATEMACHINE_API UMStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:	

	// �܂肽���߂�悤�� region���Ă�
	//---UActorComponent Interface
	#pragma region UActorComponent Interface
	// �R���|�[�l���g�������������Ƃ��ɌĂ΂��֐�(BeginPlay() ���������Ă΂��)
	virtual void InitializeComponent() override;
	// ���t���[������
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// �R���|�[�l���g�̏I�����ɌĂ΂��
	virtual void UninitializeComponent() override;
	#pragma endregion UActorComponent Interface
	//---End of UActorComponent Interface

	// �X�e�[�g�� Tick�i���t���[���X�V�j���J�n
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StartTickState();

	// �X�e�[�g�� Tick ���~
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StopTickState();

	// �J�n��Ԃ��w�肷��
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void SetEntryState(const FGameplayTag& EntryStateTag);

	// �V�����X�e�[�g��ǉ����� FMStateHandle ��Ԃ�
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

	// �����X�e�[�g���܂Ƃ߂Ēǉ�
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	TArray<FMStateHandle> AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses);

	// ����̃X�e�[�g���폜
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void RemoveState(FMStateHandle StateHandle);

	// �w�肳�ꂽ�^�O�̃X�e�[�g�ɑJ��
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	bool SwitchNextState(const FGameplayTag& NextStateTag);

	// �w�肳�ꂽ�^�O�̃X�e�[�g�����邩�𔻒�
	UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
	bool ContainsStateTag(const FGameplayTag& Tag) const;

	// ���݂̏�Ԃ��炻�̃^�O�ɑJ�ڂł��邩
	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

	// ���݂̃X�e�[�g�^�O���擾
	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	FGameplayTag GetCurrentStateTag() const;

private:
	UPROPERTY()
	FMStateMachineStateList m_stateList;	// �X�e�[�g�ꗗ

	UPROPERTY()
	TObjectPtr<UMStateInstance> m_currentState;	// ���ݗL���� UMStateInstance

	uint8 m_bIsStateMachineStarted : 1;	// �X�e�[�g�}�V�����J�n����Ă��邩�i1bit �t���O�j
	uint8 m_bCanTickStateMachine : 1;	// Tick�������L�����ǂ���
};
