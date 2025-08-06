#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InsekiClimbingObject.generated.h"

UCLASS()
class ARRANGER_API AInsekiClimbingObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AInsekiClimbingObject();

    // クライム判定用コリジョン
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* ClimbTrigger;

    // クライム面の法線（上下方向を定義）
    UFUNCTION(BlueprintCallable)
    FVector GetClimbNormal();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};