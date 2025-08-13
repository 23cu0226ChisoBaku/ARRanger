
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class ARRANGER_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bIsChasing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bIsSearch = false;
};
