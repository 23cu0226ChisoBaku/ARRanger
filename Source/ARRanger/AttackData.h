#pragma once

#include "CoreMinimal.h"
#include "AttackData.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	// �U���A�j���[�V���������^�[�W��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage = nullptr;

	// �����蔻��̔��a
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitRadius = 100.f;

	// �_���[�W
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 10;

	// �U���Ώۂ̃^�O
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetTag = "Enemy"; 
};