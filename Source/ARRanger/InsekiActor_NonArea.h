#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InsekiActor_NonArea.generated.h"

class UCapsuleComponent;

UCLASS()
class ARRANGER_API AInsekiActor_NonArea : public AActor
{
	GENERATED_BODY()

public:
	AInsekiActor_NonArea();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> Capsule;

	TObjectPtr<AActor> playerCharacter;

	UPROPERTY()
	UPrimitiveComponent* RootPrimitive;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float basePower;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float minValue;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float maxValue;

	// LaunchCharacter�Ŏg���\��̑��x��ۑ�����ϐ�
	FVector PendingLaunchVelocity;

	// �Ō�ɂԂ���������(�����l�͏\���ߋ��̎��Ԃ̂��̂ɂ���)
	float lastHitTime;

	// ���Ԋu�ŏ��������邽�߂̃N�[���^�C��
	float hitCoolTime;

	// �Ԃ��������ɒx����LaunchCharacter���ĂԂ��߂̃^�C�}�[
	FTimerHandle LaunchTimerHandle;

	FTimerHandle ResetMovementTimerHandle;

private:
	// �x���Ăяo���p�֐��iOnHit������Ă΂��j
	void PerformLaunch();

	void RestoreMovementMode();

public:
	virtual void Tick(float DeltaTime) override;

	// OnHit�֐���錾
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};