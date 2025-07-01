#include "InsekiActor.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
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

	// SphereComponentを追加し、SphereComponentをRootComponentにAttachする
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

	if (onStayFlag == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("huttobeeeeeee"));

		// プレイヤーとの方向と距離を取得
		FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
		float Distance = Direction.Size();

		// ベクトルを初期化
		FVector DirectionNorm = Direction / Distance;

		UE_LOG(LogTemp, Warning, TEXT("PushOffset size: %f"), Direction.Size());
		//repulsionFlag = true;

		if (Distance <= 200.f)
		{
			repulsionFlag = true;
		}
		if (repulsionFlag == true)
		{
			// 磁気力を発してプレイヤーを押し出す
			FVector pushForce = DirectionNorm * constProp * magneticValue * magneticValue / (Distance * Distance);

			// デバッグ確認用
			UE_LOG(LogTemp, Warning, TEXT("pushForce Value Is : %f"), pushForce.Size());

			playerCharacter->AddActorWorldOffset(pushForce, false);

			/*// ACharacterにキャストして動かす
			ACharacter* Char = Cast<ACharacter>(playerCharacter);
			Char->LaunchCharacter(pushForce, false, false);*/
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
		// 入ってきたフラグをオンに
		onStayFlag = true;
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
		// 入ってきたフラグをオフに
		onStayFlag = false;
		repulsionFlag = false;
	}
}