
#pragma once

#include "CoreMinimal.h"
#include "Enemy/ZakoAIController.h"
#include "MiddleBossAIController.generated.h"

UCLASS()
class ARRANGER_API AMiddleBossAIController : public AZakoAIController
{
	GENERATED_BODY()
	
  public:
    AMiddleBossAIController();

  protected:
    virtual void BeginPlay() override;

    virtual void StopChasing() override;

    virtual void BroadcastAlert(AActor* SeenActor) override;
};
