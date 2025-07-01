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
	playerCharacter = FoundActors[0];
}

void AInsekiActor_NonArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		ACharacter* Char = Cast<ACharacter>(playerCharacter);
		Char->LaunchCharacter(ProjectedVelocity, false, false);

		// LaunchCharacterで使う予定の速度を保存
		PendingLaunchVelocity = ProjectedVelocity;

		// 少し遅延してLaunchCharacterを呼ぶ
		//GetWorld()->GetTimerManager().SetTimer(LaunchTimerHandle, this, &AInsekiActor_NonArea::PerformLaunch, 0.02f, false);

		/*// 衝突方向ベクトル
		FVector ToPlayerDir = playerCharacter->GetActorLocation() - GetActorLocation();

		// はじき飛ばす力の大きさ（調整可能）
		const float PushPower = 50.0f;

		// 移動ベクトルを直接指定
		FVector PushVector = ToPlayerDir * PushPower;

		// ACharacterにキャストして動かす
		ACharacter* Char = Cast<ACharacter>(playerCharacter);
		Char->LaunchCharacter(PushVector, false, false);*/
	}
}

void AInsekiActor_NonArea::RestoreMovementMode()
{
	if (playerCharacter)
	{
		auto Char = Cast<ACharacter>(playerCharacter);
		if (Char)
		{
			Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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