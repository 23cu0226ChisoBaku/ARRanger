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
    // AbilitySystemComponent�ｽ�ｽﾛ托ｿｽ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilitySystemComponent* AbilitySystemComp;

public:
    UAttackComponent();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // パンチの際に呼び出される
    void StartPunch();

    // �ｽL�ｽb�ｽN�ｽﾌ際に呼び出�ｽ�ｽ�ｽ�ｽ�ｽ
    void StartKick();

    // �ｽp�ｽ�ｽ�ｽ`�ｽ�ｽAnimNotify�ｽﾌ通知�ｽ�ｽ�ｽｯ趣ｿｽ�ｽ
    UFUNCTION(BlueprintCallable)
    void PunchHitNotify();

    // �ｽL�ｽb�ｽN�ｽ�ｽAnimNotify�ｽﾌ通知�ｽ�ｽ�ｽｯ趣ｿｽ�ｽ
    UFUNCTION(BlueprintCallable)
    void KickHitNotify();

    // �ｽU�ｽ�ｽ�ｽ�ｽ�ｽI�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾛのコ�ｽ[�ｽ�ｽ�ｽo�ｽb�ｽN
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

private:
    // �ｽG�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽｹ抵ｿｽ�ｽﾌフ�ｽ�ｽ�ｽO
    bool isAttractingEnemy;

    // �ｽG�ｽ≠�ｽ�ｽ�ｽﾎゑｿｽ�ｽt�ｽ�ｽ�ｽO
    bool isBlowedAwayEnemy;

    // �ｽ�ｽ�ｽ�ｽ�ｽU�ｽ�ｽ�ｽ�ｽ�ｽﾇゑｿｽ�ｽ�ｽ�ｽﾌフ�ｽ�ｽ�ｽO
    bool isStrongAttack;

    // �ｽU�ｽ�ｽ�ｽ�ｽ�ｽﾉプ�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽ�ｽ�ｽ^�ｽ[�ｽQ�ｽb�ｽg�ｽﾌ包ｿｽ�ｽ�ｽ�ｽﾉ鯉ｿｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ
    void RotateOwnerToTarget();

    // �ｽU�ｽ�ｽ�ｽA�ｽj�ｽ�ｽ�ｽ[�ｽV�ｽ�ｽ�ｽ�ｽ�ｽﾌ再撰ｿｽ�ｽp�ｽﾖ撰ｿｽ
    void PlayAttackMontage(const FAttackData& Attack);

    // �ｽ�ｽ�ｽ�ｽ�ｽ阡ｻ�ｽ�ｽﾌ擾ｿｽ�ｽ�ｽ
    void AttackHit(const FAttackData& Attack);

    // �ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌオ�ｽ[�ｽi�ｽ[
    AARRangerCharacter* ownerPawn;

    // �ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌコ�ｽ�ｽ�ｽg�ｽ�ｽ�ｽ[�ｽ�ｽ�ｽ[
    APlayerController* ownerController;

public:
    // �ｽp�ｽ�ｽ�ｽ`�ｽf�ｽ[�ｽ^�ｽiBlueprint�ｽ�ｽ�ｽ�ｽﾝ抵ｿｽj
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData PunchData;

    // �ｽL�ｽb�ｽN�ｽf�ｽ[�ｽ^�ｽiBlueprint�ｽ�ｽ�ｽ�ｽﾝ抵ｿｽj
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData KickData;

    // �ｽU�ｽ�ｽ�ｽ�ｽ�ｽt�ｽ�ｽ�ｽO
    UPROPERTY(BlueprintReadOnly)
    bool IsAttacked;

    // �ｽq�ｽb�ｽg�ｽG�ｽt�ｽF�ｽN�ｽg�ｽp�ｽﾌア�ｽN�ｽ^�ｽ[
    UPROPERTY(EditAnywhere, Category = "Effects")
    TSubclassOf<AActor> HitEffectActor;

    // �ｽ�ｽ�ｽ�ｽ�ｽｹ抵ｿｽ�ｽt�ｽ�ｽ�ｽO�ｽ�ｽ�ｽ謫ｾ
    bool GetIsAttractingEnemy() { return isAttractingEnemy; }

    // �ｽU�ｽ�ｽ�ｽ�ｽ�ｽt�ｽ�ｽ�ｽO�ｽ�ｽ�ｽ謫ｾ
    bool GetIsAttacked() { return IsAttacked; }

    //�ｽ�ｽ�ｽU�ｽ�ｽ�ｽt�ｽ�ｽ�ｽO�ｽ�ｽ�ｽ謫ｾ
    bool GetIsStrongAttacked() { return isStrongAttack; }
};