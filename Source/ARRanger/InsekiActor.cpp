#include "InsekiActor.h"

#include "ARRangerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AInsekiActor::AInsekiActor()
	: InsekiActorMesh(nullptr)
	, Sphere(nullptr)
	, magneticValue(3.0f)
	, constProp(63300.0f)
	, onStayFlag(false)
	, repulsionFlag(false)
	, isLaunchedFlag(false)
	, playerCharacter(nullptr)
	, RootPrimitive(nullptr)
	, PlayerRoot(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponentを追加し、SphereComponentをRootComponentにAttachする
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AInsekiActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlapをBindする
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AInsekiActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AInsekiActor::OnSphereEndOverlap);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);
	playerCharacter = FoundActors[0];

	// プレイヤーのRootComponentを取得
	PlayerRoot = Cast<UPrimitiveComponent>(playerCharacter->GetRootComponent());
}

void AInsekiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
	float Distance = Direction.Size();
	if (Distance <= 250.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Oooooooooooon"));
		repulsionFlag = true;
	}

	if (repulsionFlag && playerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ugoku?"));
		FVector DirectionNorm = Direction / Distance;

		// 有効範囲を指定（ここまでなら力が働く）
		const float MaxRange = 3000.f;
		const float MinDistance = 100.f;
		const float ForceScale = constProp * magneticValue * magneticValue;

		

		if (Distance < MaxRange)
		{
			float ClampedDistance = FMath::Clamp(Distance, MinDistance, MaxRange);
			float ForceMagnitude = ForceScale / (ClampedDistance * ClampedDistance);
			FVector PushForce = DirectionNorm * ForceMagnitude;

			playerCharacter->AddActorWorldOffset(PushForce, false);

			UE_LOG(LogTemp, Warning, TEXT("Magnetic push active. Distance: %f, Force: %s"), Distance, *PushForce.ToString());
		}

		if (Distance >= MaxRange)
		{
			repulsionFlag = false;
		}
	}
}
void AInsekiActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 接触したActorがAInsekiCharacterか判定する
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("gomikasuuuuuuu"));
		RootPrimitive = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
	}
}

void AInsekiActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("kutabareeeeee"));
	// 接触したActorがAInsekiCharacterか判定する
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		RootPrimitive = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("shinebokeeeeeee"));
	}
}