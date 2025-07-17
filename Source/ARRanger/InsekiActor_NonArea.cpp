#include "InsekiActor_NonArea.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AInsekiActor_NonArea::AInsekiActor_NonArea()
	: InsekiActorMesh(nullptr)
	, Capsule(nullptr)
	, playerCharacter(nullptr)
	, RootPrimitive(nullptr)
	, basePower(10.0f)
	, minValue(0.0f)
	, maxValue(10.0f)
	, lastHitTime(-100.0f)
	, hitCoolTime(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// CapsuleComponentを追加し、CapsuleComponentをRootComponentにAttachする
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	Capsule->SetupAttachment(RootComponent);
}

void AInsekiActor_NonArea::BeginPlay()
{
	Super::BeginPlay();

	// OnHitをBindする
	Capsule->OnComponentHit.AddDynamic(this, &AInsekiActor_NonArea::OnHit);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);

	if (FoundActors.Num() > 0 && FoundActors[0] != nullptr)
	{
		playerCharacter = FoundActors[0];
	}
	else
	{
		playerCharacter = nullptr;
		UE_LOG(LogTemp, Error, TEXT("AInsekiActor_NonArea: No valid actor found with tag 'Player'"));
	}
}

void AInsekiActor_NonArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!playerCharacter)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);
		if (FoundActors.Num() > 0)
		{
			playerCharacter = FoundActors[0];
			UE_LOG(LogTemp, Warning, TEXT("Found PlayerCharacter later in Tick"));
		}
	}
}

void AInsekiActor_NonArea::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	if (currentTime - lastHitTime < hitCoolTime)
	{
		// まだクールダウン中なので処理しない
		return;
	}
	lastHitTime = currentTime;


	// プレイヤーと衝突したときの処理
	if (OtherActor == playerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("%hs called"), __func__);

		// Comment：コライダーの中心座標をとってね
		FVector ToPlayerDir = playerCharacter->GetActorLocation() - GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("To Player Direction: X:%f, Y:%f, Z:%f, Length:%f"), ToPlayerDir.X, ToPlayerDir.Y, ToPlayerDir.Z, ToPlayerDir.Size());

		FVector PlayerVelocity = OtherActor->GetVelocity();
		//UE_LOG(LogTemp, Warning, TEXT("Player Velocity: X:%f, Y:%f, Z:%f, Length:%f"),PlayerVelocity.X, PlayerVelocity.Y, PlayerVelocity.Z, PlayerVelocity.Size());

		FVector ProjectedVelocity = FVector::DotProduct(PlayerVelocity.GetSafeNormal(), ToPlayerDir.GetSafeNormal()) * ToPlayerDir;
		//UE_LOG(LogTemp, Warning, TEXT("Player ProjectedVelocity: X:%f, Y:%f, Z:%f, Length:%f"), ProjectedVelocity.X, ProjectedVelocity.Y, ProjectedVelocity.Z, ProjectedVelocity.Size());

		const float projectedVeloLength = ProjectedVelocity.Size();
		const float fixedProjectedVeloLength = FMath::Clamp(projectedVeloLength, minValue, maxValue);

		//UE_LOG(LogTemp, Warning, TEXT("Force: %f"), fixedProjectedVeloLength);

		// ベロシティを補正
		if (projectedVeloLength != fixedProjectedVeloLength)
		{
			// 物理シミュレーションでPlayerVelocity先に0になってしまった時の救済措置
			if (ProjectedVelocity.IsNearlyZero())
			{
				ProjectedVelocity = ToPlayerDir.GetSafeNormal();
			}
			ProjectedVelocity.Normalize();
			ProjectedVelocity *= fixedProjectedVeloLength;
		}

		/*UE_LOG(LogTemp, Warning, TEXT("Player Velocity: %s"), *ProjectedVelocity.ToString());

		UE_LOG(LogTemp, Warning, TEXT("Player Velocity: %f"), ProjectedVelocity.Size());*/

		// ACharacterにキャストして動かす
		/*FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
		float Distance = Direction.Size();
		FVector DirectionNorm = Direction / Distance;
		const float MinDistance = 100.f;
		const float ForceScale = 63300.0f * 5000.0f * 5000.0f;

		float ForceMagnitude = ForceScale / (Distance * Distance);
		FVector PushForce = DirectionNorm * ForceMagnitude;

		ACharacter* Char = Cast<ACharacter>(playerCharacter);
		Char->LaunchCharacter(ProjectedVelocity, false, false);*/

		ACharacter* Char = Cast<ACharacter>(playerCharacter);
		// 接触方向を取得
		FVector ImpactDirection = playerCharacter->GetActorLocation() - GetActorLocation();
		ImpactDirection.Normalize();

		// 真上から来てるかどうか判定（Z成分が正で十分大きい）
		if (ImpactDirection.Z > 0.7f) // 真上から落ちてきた場合
		{
			// プレイヤーをLaunch（Yは無視して上に飛ばす）
			FVector LaunchVelocity = FVector(0.f, 0.f, 1200.f); 
			Char->LaunchCharacter(LaunchVelocity, true, true);
		}
		else
		{
			// 横から当たった場合は吹き飛ばし（Zは少しだけ、XY方向に飛ばす）
			FVector HorizontalDir = ImpactDirection;
			HorizontalDir.Z = 0.f;
			HorizontalDir.Normalize();

			FVector KnockbackVelocity = HorizontalDir * 800.f + FVector(0.f, 0.f, 200.f);
			Char->LaunchCharacter(KnockbackVelocity, true, false);
		}
	}


}

/*void AInsekiActor_NonArea::PerformLaunch()
{
	AARRangerCharacter* Char = Cast<AARRangerCharacter>(playerCharacter);
	if (Char)
	{
		Char->LaunchCharacter(PendingLaunchVelocity, false, false);
		UE_LOG(LogTemp, Log, TEXT("LaunchCharacter called with velocity: %s"), *PendingLaunchVelocity.ToString());
	}
}*/