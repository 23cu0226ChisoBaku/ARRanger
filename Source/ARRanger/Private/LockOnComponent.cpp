#include "LockOnComponent.h"

#include "Enemy/Enemy_Zako.h"
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
	if (isLockedOn && lockedOnTarget.IsValid())
	{
		if (!IsTargetVisible(lockedOnTarget))
		{
			// ターゲットが見えなくなったらロックオン解除
			lockedOnTarget = nullptr;
            SetIsLockedOn(false);
		}

        // ロックオン中の敵が死んだら処理
        if (!lockedOnTarget.IsValid() || lockedOnTarget->isDead)
        {
            AEnemy_Zako* NewTarget = FindNearestEnemy(lockedOnTarget);
            // 新しくターゲットを設定
            if (NewTarget)
            {
                lockedOnTarget = NewTarget;
            }
            // いなければロックオン解除
            else
            {
                lockedOnTarget = nullptr;
                SetIsLockedOn(false);
            }
        }
	}
}

void ULockOnComponent::ToggleLockOn()
{
    UE_LOG(LogTemp, Warning, TEXT("Lock On Check"));

    // プレイヤーがいなければ処理しない
    if (!ownerPawn || !ownerController)
    {
        return;
    }

    if (isLockedOn)
    {
        // ロックオンを解除
        lockedOnTarget = nullptr;
        SetIsLockedOn(false);
        UE_LOG(LogTemp, Warning, TEXT("LockOn: Unlocked"));
    }
    else
    {
        AEnemy_Zako* Candidate = FindNearestEnemy();
        if (Candidate)
        {
            bool bVisible = IsTargetVisible(Candidate);
            UE_LOG(LogTemp, Warning, TEXT("ToggleLockOn: Found enemy %s, Visible=%d"),
                *Candidate->GetName(), bVisible);

            if (bVisible)
            {
                // プレイヤーから見えていればロックオン開始
                lockedOnTarget = Candidate;
                SetIsLockedOn(true);
                UE_LOG(LogTemp, Warning, TEXT("LockOn: Locked on %s"), *Candidate->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("LockOn: Candidate not visible"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LockOn: No enemy found by FindNearestEnemy()"));
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
    if (!isLockedOn || !lockedOnTarget.IsValid() || !ownerPawn)
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
            lockedOnTarget = Cast<AEnemy_Zako>(Candidate);
            return;
        }

        Checked++;
    }
}

AEnemy_Zako* ULockOnComponent::FindNearestEnemy(TWeakObjectPtr<AEnemy_Zako> IgnoreActor)
{
    // プレイヤーがいなければ処理しない
    if (!ownerPawn)
    {
        return nullptr;
    }

    // ワールドの敵を取得(To Do：一定範囲内の敵を取得するように修正)
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), enemyTag, Enemies);

    AEnemy_Zako* NearestEnemy = nullptr;
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
            NearestEnemy = Cast<AEnemy_Zako>(Enemy);
        }
    }

    return NearestEnemy;
}

bool ULockOnComponent::IsTargetVisible(TWeakObjectPtr<AActor> Target)
{
    // ターゲットまたはプレイヤーがいなければ処理しない
    if (!Target.IsValid() || !ownerController)
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
    Params.AddIgnoredActor(Target.Get());

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TargetLocation, ECC_Visibility, Params);

    // ヒットしていてかつそのActorがTargetでなければ、
    // 視界を遮られたとみなす
    if (bHit && HitResult.GetActor() != Target)
    {
        return false;
    }

    return true;
}