#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "ARRangerCharacter.generated.h"

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

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MouseLookAction;

	// ���b�N�I���A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LockOnAction;

public:

	/** �R���X�g���N�^ */
	AARRangerCharacter();	

protected:

	/** ���̓A�N�V�����̃o�C���f�B���O������������ */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** �ړ����͂̂��߂ɌĂяo����� */
	void Move(const FInputActionValue& Value);

	/** ���͂����߂� */
	void Look(const FInputActionValue& Value);

private:
	// ���b�N�I�����t���O
	bool bIsLockedOn;

	// ���b�N�I���ؑ֊֐�
	void ToggleLockOn();

	// �X�e�B�b�N���͂œG�ؑցi�E�X�e�B�b�N��X���Ȃǁj
	void SwitchTarget(float Value);

	// ���b�N�I���\�ȓG������
	AActor* FindNearestEnemy();

public:

	/** �R���g���[���܂���UI�C���^�[�t�F�[�X����̈ړ����͂��������� */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** �R���g���[���܂���UI�C���^�[�t�F�[�X����̃��b�N���͂��������� */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** �R���g���[���܂���UI�C���^�[�t�F�[�X�̂ǂ��炩��ł��A�����ꂽ�W�����v���͂��������� */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** �R���g���[���܂���UI�C���^�[�t�F�[�X�̂ǂ��炩��ł��A�����ꂽ�W�����v���͂��������� */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	// ���b�N�I���Ώ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

public:
	virtual void Tick(float DeltaTime) override;

	/** CameraBoom�T�u�I�u�W�F�N�g��Ԃ� **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** FollowCamera�T�u�I�u�W�F�N�g��Ԃ� **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};