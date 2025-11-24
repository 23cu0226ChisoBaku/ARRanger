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

  UPROPERTY(VisibleAnywhere)
  class UBoxComponent* ClimbTrigger;

  UFUNCTION(BlueprintCallable)
  FVector GetClimbNormal();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};