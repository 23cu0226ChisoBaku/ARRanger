#pragma once

#include "AttackData.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InsekiClimbingObject.h"
#include "Logging/LogMacros.h"
#include "PlayerObservation/IObservableSubjectInterface.h"

#include "ARRangerCharacter.generated.h"

class UAnimMontage;
class USkeletalMesh;
class UInputAction;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

// �v���C���[�̕ϐg���(���́A�˗�)
UENUM(BlueprintType)
enum class EGravityType : uint8
{
	Attractive,
	Repulsive
};

/**
 *  �V���v���Ńv���C���[������\�ȎO�l�̎��_�L�����N�^�[
 *  ����\�ȋO���J�����̎���
 */
UCLASS(abstract)
class AARRangerCharacter : public ACharacter,
                           public IObservableSubjectInterface
{
	GENERATED_BODY()

	
protected:
	virtual void BeginPlay() override;

  // 麦
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// �W�����v�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	// �ړ��A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	// �R��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ClimbAction;

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

	// �L�b�N�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* KickAction;

	// �ϐg�A�N�V����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* TransformAction;

	// 引力クライムフラグ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isClimbed;

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
	bool isLockedOn;

	// ���b�N�I�����G�؂�ւ��̉\�t���O
	bool isAbleToSwitchTarget;

	// �R��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climeb", meta = (AllowPrivateAccess = "true"))
	bool isClimb;

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
	void StartPunch();

	// �L�b�N�̍ۂɌĂяo�����
	void Kick();

	// �U���A�j���[�V�����̍Đ��p�֐�
	void PlayAttackMontage(const FAttackData& Attack);

	// �����蔻��̏���
	void AttackHit(const FAttackData& Attack);

	// �ϐg�̍ۂɌĂяo�����
	void Transform();

	// �_�b�V�����J�������؂�ւ����͂�臒l�i�������ݎ��j
	float dashStartThreshold;

	// �������͂��ɂ߂���_�b�V������������p�̐��l
	float dashEndThreshold;

	// �G�������񂹒��̃t���O
	bool isAttractingEnemy;

	// �����U�����ǂ����̃t���O
	bool isStrongAttack;

	// 現在歩いているオブジェクトの表面
	UPROPERTY()
	AInsekiClimbingObject* currentClimbSurface;

	// 引力クライムオブジェクトに触れた際に呼び出される
	UFUNCTION()
	void OnClimbSurfaceOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// 引力クライムを開始する際に呼び出される
	void StartClimbing(AInsekiClimbingObject* ClimbActor);

	// 引力クライムをやめる際に呼び出される
	void StopClimbing();

public:

	// �R���g���[���܂���UI�C���^�[�t�F�[�X����̈ړ����͂���������
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	// �R��
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoClimb(float Right, float Up);

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

	UFUNCTION(BlueprintPure, Category = "AR|Player")
	float GetDefaultArmLength() const { return DefaultArmLength; }

	UFUNCTION(BlueprintPure, Category = "AR|Player")
	float GetDashArmLength() const { return DashArmLength; }

	// ���͗p�v���C���[���b�V��
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* AttractiveMesh;

	// �˗͗p�v���C���[���b�V��
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* RepulsiveMesh;

	// ���b�N�I���Ώ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	// ���b�N�I���\����
	UPROPERTY(EditAnywhere, Category = "LockOn")
	float maxLockOnDistance;

	// �_�b�V�����t���O
	UPROPERTY(BlueprintReadWrite)
	bool isDashed;

	// �p���`�f�[�^�iBlueprint����ݒ�j
	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData PunchData;

	// �L�b�N�f�[�^�iBlueprint����ݒ�j
	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData KickData;

	// �U�����t���O
	UPROPERTY(BlueprintReadOnly)
	bool isAttacked;

	// ���݂̃v���C���[�̕ϐg���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	EGravityType CurrentGravityType;

public:
	virtual void Tick(float DeltaTime) override;


	// ���݂̃v���C���[�̃��[�h���擾
	EGravityType GetCurrentGravityType();

  // 麦
  bool bIsJumping = false;

  UFUNCTION()
  void LandedToGround(const FHitResult& Hit)
  {
    bIsJumping = false;
  }
};