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
	// プレイヤーの現在のモードを確認（引力 or 斥力）
	// 取得できなければ処理しない
	AARRangerCharacter* player = Cast<AARRangerCharacter>(playerCharacter);
	if (!player)
	{
		return;
	}

	// 斥力状態のときのみ吹っ飛ばす
	if (player->GetCurrentGravityType() == EGravityType::Attractive)
	{
		// 引力状態の場合はなにもしない
		UE_LOG(LogTemp, Warning, TEXT("Player is in Attractive mode: No knockback"));
		return;
	}
	else if (player->GetCurrentGravityType() == EGravityType::Repulsive)
	{
		// 接触方向を取得
		FVector ImpactDirection = playerCharacter->GetActorLocation() - GetActorLocation();
		ImpactDirection.Normalize();

		// 真上から来てるかどうか判定(Z成分が正で十分大きい)
		// 真上から落ちてきた場合
		if (ImpactDirection.Z > 0.7f)
		{
			// プレイヤーの進行方向（速度ベクトル）を取得
			FVector Velocity = player->GetVelocity();

			// 入力があるかどうかチェック（移動しているか）
			if (Velocity.SizeSquared() > KINDA_SMALL_NUMBER)
			{
				// 前方向（進行方向）に向かってジャンプ
				FVector ForwardDir = Velocity.GetSafeNormal();

				// より高く＆爆発的に飛ばす
				FVector LaunchVelocity = ForwardDir * 1200.f + FVector(0.f, 0.f, 800.f);
				player->LaunchCharacter(LaunchVelocity, true, true);

				UE_LOG(LogTemp, Warning, TEXT("Repulsion forward launch: %s"), *LaunchVelocity.ToString());
			}
			else
			{
				// 動いてなければ真上に爆発ジャンプ
				FVector LaunchVelocity = FVector(0.f, 0.f, 1400.f);
				player->LaunchCharacter(LaunchVelocity, true, true);

				UE_LOG(LogTemp, Warning, TEXT("Repulsion vertical launch only (no input)"));
			}
		}
		else
		{
			// 横から当たった場合は吹き飛ばし(Zは少しだけ、XY方向に飛ばす)
			FVector HorizontalDir = ImpactDirection;
			HorizontalDir.Z = 0.f;
			HorizontalDir.Normalize();

			FVector KnockbackVelocity = HorizontalDir * 800.f + FVector(0.f, 0.f, 200.f);
			player->LaunchCharacter(KnockbackVelocity, true, false);
		}
	}
}