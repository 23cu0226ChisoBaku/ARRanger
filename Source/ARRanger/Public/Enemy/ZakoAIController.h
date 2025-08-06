#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ZakoAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class ARRANGER_API AZakoAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZakoAIController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardData* BlackboardAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	// ブラックボードのキー名をハードコードしないように、FNameで保持
	UPROPERTY(EditDefaultsOnly, Category = "Blackboard Keys")
	FName TargetActorKey = "TargetActor";

private:
	// 認識イベントのコールバック関数
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 近くの味方に警告をブロードキャストする関数
	void BroadcastAlert(AActor* SeenActor);
};