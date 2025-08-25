#pragma once

#include "AttackData.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AttackComponent.generated.h"

class AARRangerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARRANGER_API UAttackComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    // AbilitySystemComponent��ۑ�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilitySystemComponent* AbilitySystemComp;

public:
    UAttackComponent();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


    // // IAbilitySystemInterface �̕K�{����
    // virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

    // �p���`�̍ۂɌĂяo�����
    void StartPunch();

    // �L�b�N�̍ۂɌĂяo�����
    void StartKick();

    // �p���`��AnimNotify�̒ʒm���󂯎��
    UFUNCTION(BlueprintCallable)
    void PunchHitNotify();

    // �L�b�N��AnimNotify�̒ʒm���󂯎��
    UFUNCTION(BlueprintCallable)
    void KickHitNotify();

    // �U�����I������ۂ̃R�[���o�b�N
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

private:
    // �G�������񂹒��̃t���O
    bool isAttractingEnemy;

    // �G�𐁂���΂��t���O
    bool isBlowedAwayEnemy;

    // �����U�����ǂ����̃t���O
    bool isStrongAttack;

    // �U�����Ƀv���C���[���^�[�Q�b�g�̕����Ɍ�������
    void RotateOwnerToTarget();

    // �U���A�j���[�V�����̍Đ��p�֐�
    void PlayAttackMontage(const FAttackData& Attack);

    // �����蔻��̏���
    void AttackHit(const FAttackData& Attack);

    // �v���C���[�̃I�[�i�[
    AARRangerCharacter* ownerPawn;

    // �v���C���[�̃R���g���[���[
    APlayerController* ownerController;

public:
    // �p���`�f�[�^�iBlueprint����ݒ�j
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData PunchData;

    // �L�b�N�f�[�^�iBlueprint����ݒ�j
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData KickData;

    // �U�����t���O
    UPROPERTY(BlueprintReadOnly)
    bool IsAttacked;

    // �q�b�g�G�t�F�N�g�p�̃A�N�^�[
    UPROPERTY(EditAnywhere, Category = "Effects")
    TSubclassOf<AActor> HitEffectActor;

    // �����񂹒��t���O���擾
    bool GetIsAttractingEnemy() { return isAttractingEnemy; }

    // �U�����t���O���擾
    bool GetIsAttacked() { return IsAttacked; }

    //���U���t���O���擾
    bool GetIsStrongAttacked() { return isStrongAttack; }
};