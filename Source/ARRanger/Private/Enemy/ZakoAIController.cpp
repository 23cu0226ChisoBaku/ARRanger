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
#include "TimerManager.h"

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

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 150.0f;

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

	if (BlackboardAsset && BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);

		// 初期テスト用
		//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		//GetBlackboardComponent()->SetValueAsObject(TargetActorKey, PlayerPawn);
	}
}

void AZakoAIController::StopChasing()
{
	// 3秒経過後、ブラックボードの情報をクリアして追跡を停止
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->ClearValue(TargetActorKey);
		BB->ClearValue("IsPlayerDetected");

		// パトロールに戻るためのロジックがあればここに追加
	}
}

void AZakoAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !Actor->ActorHasTag("Player"))
		return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// プレイヤーを発見した
		BB->SetValueAsObject(TargetActorKey, Actor);
		BB->SetValueAsBool("IsPlayerDetected", true);

		// 周囲の仲間に警告を送る
		BroadcastAlert(Actor);

		// 追跡停止タイマーをクリア
		GetWorld()->GetTimerManager().ClearTimer(LostSightTimerHandle);
	}
	else
	{
		// プレイヤーを見失った
		// 最後に発見した場所を記憶するために、TargetActorKeyはクリアしない
		// BB->ClearValue(TargetActorKey);

		BB->SetValueAsBool("IsPlayerDetected", false);

		// 2秒後に追跡を停止するタイマーを開始
		GetWorld()->GetTimerManager().SetTimer(LostSightTimerHandle, this, &AZakoAIController::StopChasing, 2.0f, false);
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

	if (!bHit || Overlaps.Num() == 0) return;

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
					BB->SetValueAsObject("TargetActor", SeenActor);
					BB->SetValueAsBool("IsPlayerDetected", true);
					BB->SetValueAsBool("IsInAlertState", true); 
				}
			}
		}
	}
}
