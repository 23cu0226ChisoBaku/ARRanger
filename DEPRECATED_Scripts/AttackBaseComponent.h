#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AttackBaseComponent.generated.h"

class AARRangerCharacter;
class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UAttackBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackBaseComponent();

protected:
	virtual void BeginPlay() override;

	// AbilitySystemComponent��ۑ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComp;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// �v���C���[��ۑ�
	AARRangerCharacter* ownerPawn = nullptr;

	// �v���C���[�̃R���g���[���[���ۑ�
	APlayerController* ownerController = nullptr;

	// �U�����t���O
	bool bIsAttacked = false;

	// ���U���t���O
	bool bIsStrongAttacked = false;

	// �G������΂��t���O
	bool bIsBlowedAwayEnemy = false;

	// �G�����񂹃t���O
	bool bIsAttractingEnemy = false;

public:
	// �����񂹒��t���O���擾
	bool GetIsAttractingEnemy() { return bIsAttractingEnemy; }

	// �����񂹒��t���O���Z�b�g
	void SetIsAttractingEnemy(bool IsAttracting) { bIsAttractingEnemy = IsAttracting; }

	// �G������΂��t���O���Z�b�g
	void SetIsBlowedAwayEnemy(bool IsBlowedAwayEnemy) { bIsBlowedAwayEnemy = IsBlowedAwayEnemy; }

	// �U�����t���O���擾
	bool GetIsAttacked() { return bIsAttacked; }

	// �U�����t���O���Z�b�g
	void SetIsAttacked(bool IsAttacked) { bIsAttacked = IsAttacked; }

	// ���U���t���O���擾
	bool GetIsStrongAttacked() { return bIsStrongAttacked; }

	// ���U�����t���O���Z�b�g
	void SetIsStrongAttacked(bool IsStrongAttacked) { bIsStrongAttacked = IsStrongAttacked; }

	//�@�v���C���[��G�̕����Ɍ�������
	void RotateOwnerToTarget();


	// Montage�I���C�x���g
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};