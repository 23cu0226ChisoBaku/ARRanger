
#pragma once

#include "Enemy/Enemy_Zako.h"

#include "Enemy_MiddleBoss.generated.h"

UCLASS()
class ARRANGER_API AEnemy_MiddleBoss : public AEnemy_Zako
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float CurrentSpeed;
};
