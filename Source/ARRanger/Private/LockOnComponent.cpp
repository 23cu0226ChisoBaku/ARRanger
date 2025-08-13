#include "LockOnComponent.h"

#include "Kismet/GameplayStatics.h"

ULockOnComponent::ULockOnComponent()
	: maxLockOnDistance(1500.0f)
	, isLockedOn(false)
	, lockedOnTarget(nullptr)
	, enemyTag("Enemy")
	, ownerPawn(nullptr)
	, ownerController(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーとそのコントローラーを取得
	ownerPawn = Cast<APawn>(GetOwner());
	ownerController = ownerPawn ? Cast<APlayerController>(ownerPawn->GetController()) : nullptr;
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ロックオン中に処理
	if (isLockedOn && lockedOnTarget)
	{
		if (!IsTargetVisible(lockedOnTarget))
		{
			// ターゲットが見えなくなったらロックオン解除
			lockedOnTarget = nullptr;
			isLockedOn = false;
		}

        // ロックオン中の敵が消えたら処理
        if (!IsValid(lockedOnTarget) || lockedOnTarget->IsActorBeingDestroyed())
        {
            AActor* NewTarget = FindNearestEnemy(lockedOnTarget);
            // 新しくターゲットを設定
            if (NewTarget)
            {
                lockedOnTarget = NewTarget;
            }
            // いなければロックオン解除
            else
            {
                lockedOnTarget = nullptr;
                isLockedOn = false;
            }
        }
	}
}

void ULockOnComponent::ToggleLockOn()
{
    // プレイヤーがいなければ処理しない
    if (!ownerPawn || !ownerController)
    {
        return;
    }

    if (isLockedOn)
    {
        // ロックオン解除
        lockedOnTarget = nullptr;
        isLockedOn = false;
    }
    else
    {
        // 最も近い敵を探してロックオン
        AActor* Candidate = FindNearestEnemy();
        if (Candidate && IsTargetVisible(Candidate))
        {
            lockedOnTarget = Candidate;
            isLockedOn = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LockOn: No visible enemy found to lock on."));
        }
    }
}

void ULockOnComponent::SwitchTargetRight()
{
    SwitchTarget(true);
}

void ULockOnComponent::SwitchTargetLeft()
{
    SwitchTarget(false);
}

void ULockOnComponent::SwitchTarget(bool bRight)
{
    // ロックオン中でない、またはプレイヤーがいなければ処理しない
    if (!isLockedOn || !lockedOnTarget || !ownerPawn)
    {
        return;
    }

    // ワールドの敵を取得(To Do：一定範囲内の敵を取得するように修正)
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), enemyTag, Enemies);

    // 敵が複数体いなければ処理しない
    if (Enemies.Num() <= 1)
    {
        return;
    }

    // 敵がいなければ処理しない
    int32 CurrentIndex = Enemies.IndexOfByKey(lockedOnTarget);
    if (CurrentIndex == INDEX_NONE)
    {
        return;
    }

    const FVector MyLocation = ownerPawn->GetActorLocation();
    const int32 EnemyCount = Enemies.Num();
    int32 Index = CurrentIndex;
    int32 Checked = 0;

    while (Checked < EnemyCount)
    {
        // その他の敵を判定
        Index = bRight ? (Index + 1) % EnemyCount : (Index - 1 + EnemyCount) % EnemyCount;

        // 現在の敵になったら処理をやめる
        if (Index == CurrentIndex)
        {
            break;
        }

        AActor* Candidate = Enemies[Index];
        if (!Candidate)
        {
            Checked++;
            continue;
        }

        float Distance = FVector::Dist(MyLocation, Candidate->GetActorLocation());
        if (Distance <= maxLockOnDistance && IsTargetVisible(Candidate))
        {
            lockedOnTarget = Candidate;
            return;
        }

        Checked++;
    }
}

AActor* ULockOnComponent::FindNearestEnemy(AActor* IgnoreActor)
{
    // プレイヤーがいなければ処理しない
    if (!ownerPawn)
    {
        return nullptr;
    }

    // ワールドの敵を取得(To Do：一定範囲内の敵を取得するように修正)
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), enemyTag, Enemies);

    AActor* NearestEnemy = nullptr;
    float MinDistSq = FLT_MAX;
    FVector MyLocation = ownerPawn->GetActorLocation();
    float MaxDistSq = maxLockOnDistance * maxLockOnDistance;

    for (AActor* Enemy : Enemies)
    {
        if (!Enemy || Enemy == IgnoreActor)
        {
            continue;
        }

        float DistSq = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());
        if (DistSq <= MaxDistSq && DistSq < MinDistSq && IsTargetVisible(Enemy))
        {
            MinDistSq = DistSq;
            NearestEnemy = Enemy;
        }
    }

    return NearestEnemy;
}

bool ULockOnComponent::IsTargetVisible(AActor* Target)
{
    // ターゲットまたはプレイヤーがいなければ処理しない
    if (!Target || !ownerController)
    {
        return false;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    ownerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // 少し頭上を狙う
    FVector TargetLocation = Target->GetActorLocation() + FVector(0, 0, 50.f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    // プレイヤー自身とターゲットは無視
    Params.AddIgnoredActor(GetOwner());
    Params.AddIgnoredActor(Target);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TargetLocation, ECC_Visibility, Params);

    // ヒットしていてかつそのActorがTargetでなければ、
    // 視界を遮られたとみなす
    if (bHit && HitResult.GetActor() != Target)
    {
        return false;
    }

    return true;
}