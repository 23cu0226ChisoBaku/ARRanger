#include "Enemy/ZakoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Player/ARPlayerState.h"
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
#include "Pawn/ARPawnInitComponent.h"

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

  // Use Player State to activate ASC
  bWantsPlayerState = true;
}

void AZakoAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardAsset && BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AZakoAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset && BlackboardAsset)
	{
		UBlackboardComponent* BB = nullptr;
		if (UseBlackboard(BlackboardAsset, BB))
		{
			RunBehaviorTree(BehaviorTreeAsset);

			if (AEnemy_Zako* Zako = Cast<AEnemy_Zako>(InPawn))
			{
				BB->SetValueAsFloat(TEXT("PreferredDistance"), Zako->PreferredDistance);
				UE_LOG(LogTemp, Warning, TEXT("PreferredDistance set to %f"), Zako->PreferredDistance);
			}
		}
	}

  if (AARPlayerState* ARPS = GetPlayerState<AARPlayerState>())
  {
    if (UARPawnInitComponent* PIC = ::Cast<UARPawnInitComponent>(InPawn->GetComponentByClass(UARPawnInitComponent::StaticClass())))
    {
      PIC->InitializeAbilitySystem(ARPS->GetARAbilitySystemComponent(), ARPS); 
  
      PIC->InitializeChargeAttack(ARPS->GetARChargeAttackComponent());
    }
  }
}

void AZakoAIController::StopChasing()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->ClearValue(TargetActorKey);
		BB->ClearValue("IsPlayerDetected");
	}
}

void AZakoAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if ((Actor == nullptr) || !Actor->ActorHasTag("Player"))
  {
		return;
  }

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
  {
    return;
  }

	if (Stimulus.WasSuccessfullySensed())
	{
		BB->SetValueAsObject(TargetActorKey, Actor);
		BB->SetValueAsBool("IsPlayerDetected", true);

		BroadcastAlert(Actor);

		GetWorld()->GetTimerManager().ClearTimer(LostSightTimerHandle);
	}
	else
	{
		BB->SetValueAsBool("IsPlayerDetected", false);

		GetWorld()->GetTimerManager().SetTimer(LostSightTimerHandle, this, &AZakoAIController::StopChasing, 2.0f, false);
	}
}

void AZakoAIController::BroadcastAlert(AActor* SeenActor)
{
	if (SeenActor == nullptr)
  {
    return;
  }

	APawn* SelfPawn = GetPawn();
	if (SelfPawn == nullptr)
  {
    return;
  } 

	const float AlertRadius = 500.0f;
	const FVector Origin = SelfPawn->GetActorLocation();

	TArray<FOverlapResult> Overlaps{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(SelfPawn);

	const bool bHit = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(AlertRadius),
		Params
	);

	if (!bHit)
  {
    return;
  }
    
	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* OtherActor = Result.GetActor();
		if ((OtherActor == nullptr) || (OtherActor == SelfPawn))
    { 
      continue;
    }

    // Notify all allies near enemy
		if (AEnemy_Zako* AllyChar = Cast<AEnemy_Zako>(OtherActor))
		{
			if (AAIController* AllyAI = Cast<AAIController>(AllyChar->GetController()))
			{
				if (UBlackboardComponent* BB = AllyAI->GetBlackboardComponent())
				{
					BB->SetValueAsObject(TargetActorKey, SeenActor);
					BB->SetValueAsBool("IsPlayerDetected", true);
					BB->SetValueAsBool("IsInAlertState", true); 
				}
			}
		}
	}
}
