#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ARRangerAnimInstance.generated.h"

UCLASS()
class ARRANGER_API UARRangerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbUpSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbRightSpeed;
};