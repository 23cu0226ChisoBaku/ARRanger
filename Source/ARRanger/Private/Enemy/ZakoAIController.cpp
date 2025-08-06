#include "Enemy/ZakoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "WorldCollision.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Enemy/Enemy_Zako.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionTypes.h"

#include "Kismet/GameplayStatics.h"

AZakoAIController::AZakoAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAsset(TEXT("/Game/Enemy/BB_EnemyZako.BB_EnemyZako"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/Enemy/BT_EnemyZako.BT_EnemyZako"));

	if (BBAsset.Succeeded()) {
		BlackboardAsset = BBAsset.Object;
	}

	if (BTAsset.Succeeded()) {
		BehaviorTreeAsset = BTAsset.Object;
	}

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 1800.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AZakoAIController::OnTargetPerceptionUpdated);

}

void AZakoAIController::BeginPlay()
{
	Super::BeginPlay();

	// 修正: UseBlackboard関数は、内部でコントローラーにBlackboardComponentを関連付けます。
	// その後、GetBlackboardComponent()でアクセスできます。
	if (BlackboardAsset && BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
		UE_LOG(LogTemp, Warning, TEXT("RunBT!!"));
		// テスト用にプレイヤーをターゲットに設定
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		Blackboard->SetValueAsObject("TargetActor", PlayerPawn);

	}
}

void AZakoAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !Actor->ActorHasTag("Player"))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsObject(TargetActorKey, Actor);
		}
		BroadcastAlert(Actor);
	}
	else
	{
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->ClearValue(TargetActorKey);
		}
	}
}

void AZakoAIController::BroadcastAlert(AActor* SeenActor)
{
	if (!SeenActor) return;
	APawn* SelfPawn = GetPawn();
	if (!SelfPawn) return;

	const float AlertRadius = 1000.0f;
	FVector Origin = SelfPawn->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SelfPawn);

	bool bHit = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(AlertRadius),
		Params
	);

	if (!bHit) return;

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* OtherActor = Result.GetActor();
		if (!OtherActor || OtherActor == SelfPawn) continue;

		if (AEnemy_Zako* AllyChar = Cast<AEnemy_Zako>(OtherActor))
		{
			if (AAIController* AllyAI = Cast<AAIController>(AllyChar->GetController()))
			{
				if (UBlackboardComponent* BB = AllyAI->GetBlackboardComponent())
				{
					BB->SetValueAsObject(TargetActorKey, SeenActor);
				}
			}
		}
	}
}