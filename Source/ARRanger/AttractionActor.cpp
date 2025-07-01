#include "AttractionActor.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AAttractionActor::AAttractionActor()
	: constProp(25.0f)
	, magneticValue(20.0f)
	, onStayFlag(false)
	, repulsionFlag(false)
	, playerCharacter(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponentを追加し、BoxComponentをRootComponentにAttachする
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AAttractionActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlapをBindする
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAttractionActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAttractionActor::OnSphereEndOverlap);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);
	playerCharacter = FoundActors[0];
}

void AAttractionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (onStayFlag == true)
	{
		// プレイヤーとの方向と距離を取得
		FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
		float Distance = Direction.Size();
		UE_LOG(LogTemp, Warning, TEXT("PushOffset size: %f"), Direction.Size());

		FVector DirectionNorm = Direction / Distance;

		// 磁気力を発してプレイヤーを引き込む
		FVector pushForce = DirectionNorm * constProp * magneticValue * magneticValue / (Distance * Distance);
		playerCharacter->AddActorWorldOffset(-pushForce, true);
	}

}

void AAttractionActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 接触したActorがAInsekiCharacterか判定する
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		// 入ってきたフラグをオンに
		onStayFlag = true;
	}
}

void AAttractionActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 接触したActorがAInsekiCharacterか判定する
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("tasukattaaaaaa"));
		// 入ってきたフラグをオフに
		onStayFlag = false;
		repulsionFlag = false;
	}
}