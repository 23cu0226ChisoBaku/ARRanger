
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

    // 中ボス専用の追跡停止処理（雑魚より長めにするなど）
    virtual void StopChasing() override;

    // 中ボス専用の警告処理（雑魚への指示を送るなど） 
    virtual void BroadcastAlert(AActor* SeenActor) override;
};
