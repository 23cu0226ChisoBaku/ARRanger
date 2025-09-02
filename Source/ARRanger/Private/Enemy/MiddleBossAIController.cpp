
#include "Enemy/MiddleBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy/Enemy_Zako.h"

AMiddleBossAIController::AMiddleBossAIController()
{
    // 中ボス専用のBB/BTをロード
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAsset(TEXT("/Game/Enemy/BB_MiddleEnemy.BB_MiddleEnemy"));
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/Enemy/BT_MiddleEnemy.BT_MiddleEnemy"));

    if (BBAsset.Succeeded()) {
        BlackboardAsset = BBAsset.Object;
    }

    if (BTAsset.Succeeded()) {
        BehaviorTreeAsset = BTAsset.Object;
    }

    // 視界を雑魚より広くする
    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 180.0f;
}

void AMiddleBossAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BlackboardAsset && BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AMiddleBossAIController::StopChasing()
{
    // 中ボスはすぐには諦めない → 5秒で忘れる
    //UE_LOG(LogTemp, Warning, TEXT("Boss StopChasing called"));

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB)
    {
        GetWorld()->GetTimerManager().SetTimer(
            LostSightTimerHandle,[BB]()
            {
                BB->ClearValue("TargetActor");
                BB->SetValueAsBool("IsPlayerDetected", false);
            },
            5.0f,
            false
        );
    }
}

void AMiddleBossAIController::BroadcastAlert(AActor* SeenActor)
{
    // 雑魚を集めるために親クラスの処理を呼んでもいい
    Super::BroadcastAlert(SeenActor);

    // 追加で「怒り状態」フラグを立てたりする
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsBool("IsEnraged", true);
    }
}