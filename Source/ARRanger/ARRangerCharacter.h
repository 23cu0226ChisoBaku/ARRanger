#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "ARRangerCharacter.generated.h"

class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  �V���v���Ńv���C���[������\�ȎO�l�̎��_�L�����N�^�[
 *  ����\�ȋO���J�����̎���
 */
UCLASS(abstract)
class AARRangerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** �J�������L�����N�^�[�̔w��ɔz�u����J�����u�[�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** �t�H���[�J���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:
	virtual void BeginPlay() override;

	// �W�����v�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	// �ړ��A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	// ���_��]�A�N�V����(�Q�[���p�b�h)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	// ���_��]�A�N�V����(�}�E�X)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MouseLookAction;

	// ���b�N�I���A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LockOnAction;

	// ���b�N�I�����^�[�Q�b�g�؂�ւ��A�N�V����(���̃^�[�Q�b�g)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetRightAction;

	// ���b�N�I�����^�[�Q�b�g�؂�ւ��A�N�V����(�O�̃^�[�Q�b�g)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetLeftAction;

	// �p���`�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PunchAction;

	// �p���`�A�j���[�V���������^�[�W��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* PunchMontage;

	// �L�b�N�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* KickAction;

	// �L�b�N�A�j���[�V���������^�[�W��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* KickMontage;

public:

	// �R���X�g���N�^
	AARRangerCharacter();	

protected:

	// ���̓A�N�V�����̃o�C���f�B���O������������
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// �ړ����͂̂��߂ɌĂяo�����
	void Move(const FInputActionValue& Value);

	// ���͂����߂�
	void Look(const FInputActionValue& Value);

private:
	// ���b�N�I�����t���O
	bool bIsLockedOn;

	// ���b�N�I�����G�؂�ւ��̉\�t���O
	bool isAbleToSwitchTarget;

	// ���Ƃ��Ƃ̃J�����ƃv���C���[�̋���
	float DefaultArmLength;

	// �_�b�V�����ɋ߂Â��鋗��
	float DashArmLength; 

	// ��ԑ��x
	float ArmLengthInterpSpeed; 

	// ���b�N�I���ؑ֊֐�
	void ToggleLockOn();

	// �\���{�^���E���������ۂɌĂяo�����
	void SwitchTargetRight();

	// �\���{�^�������������ۂɌĂяo�����
	void SwitchTargetLeft();

	// ���b�N�I�����^�[�Q�b�g�؂�ւ��֐�(�����ɂ���đO��ɐ؂�ւ�)
	void SwitchTarget(bool isPressedRight);

	// ���b�N�I���\�ȓG������
	AActor* FindNearestEnemy(AActor* IgnoreActor = nullptr);

	// �p���`�̍ۂɌĂяo�����
	void Punch();

	// �L�b�N�̍ۂɌĂяo�����
	void Kick();

public:

	// �R���g���[���܂���UI�C���^�[�t�F�[�X����̈ړ����͂���������
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	// �R���g���[���܂���UI�C���^�[�t�F�[�X����̃��b�N���͂���������
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	// �R���g���[���܂���UI�C���^�[�t�F�[�X�̂ǂ��炩��ł��A�����ꂽ�W�����v���͂���������
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	// �R���g���[���܂���UI�C���^�[�t�F�[�X�̂ǂ��炩��ł��A�����ꂽ�W�����v���͂���������
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	// �p���`��AnimNotify�̒ʒm���󂯎��
	UFUNCTION(BlueprintCallable)
	void PunchHitNotify();

	// �L�b�N��AnimNotify�̒ʒm���󂯎��
	UFUNCTION(BlueprintCallable)
	void KickHitNotify();

	// �U�����I������ۂ̃R�[���o�b�N
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

	// ���b�N�I���Ώ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	// ���b�N�I���\����
	UPROPERTY(EditAnywhere, Category = "LockOn")
	float maxLockOnDistance;

	// �_�b�V�����t���O
	UPROPERTY(BlueprintReadWrite)
	bool isDashed;

	// �ړ����͂�臒l(����𒴂���ƃ_�b�V���ɑJ�ڂ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveThreshold;

	// �U����
	int attackPower;

	// �p���`�̓����蔻��p
	UPROPERTY(EditAnywhere, Category = "Combat")
	float punchRadius;

	// �L�b�N�̓����蔻��p
	UPROPERTY(EditAnywhere, Category = "Combat")
	float kickRadius;

	// �U�����t���O
	UPROPERTY(BlueprintReadOnly)
	bool isAttacked;

public:
	virtual void Tick(float DeltaTime) override;

	// CameraBoom�T�u�I�u�W�F�N�g��Ԃ�
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera�T�u�I�u�W�F�N�g��Ԃ�
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};