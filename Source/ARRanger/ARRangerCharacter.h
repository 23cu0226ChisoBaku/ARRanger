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

	// �p���`�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PunchAction;

	// �p���`�A�j���[�V���������^�[�W��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* PunchMontage;

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

	// ���Ƃ��Ƃ̃J�����ƃv���C���[�̋���
	float DefaultArmLength;

	// �_�b�V�����ɋ߂Â��鋗��
	float DashArmLength; 

	// ��ԑ��x
	float ArmLengthInterpSpeed; 

	// ���b�N�I���ؑ֊֐�
	void ToggleLockOn();

	// �X�e�B�b�N���͂œG�ؑցi�E�X�e�B�b�N��X���Ȃǁj
	void SwitchTarget(float Value);

	// ���b�N�I���\�ȓG������
	AActor* FindNearestEnemy();

	// �p���`�̍ۂɌĂяo�����
	void Punch();

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

	// ���b�N�I���Ώ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	// �_�b�V�����t���O
	UPROPERTY(BlueprintReadWrite)
	bool bIsDashing;

	// �ړ����͂�臒l(����𒴂���ƃ_�b�V���ɑJ�ڂ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveThreshold;

	// �p���`�̓����蔻��p
	UPROPERTY(EditAnywhere, Category = "Combat")
	float punchRadius;

public:
	virtual void Tick(float DeltaTime) override;

	// CameraBoom�T�u�I�u�W�F�N�g��Ԃ�
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera�T�u�I�u�W�F�N�g��Ԃ�
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};