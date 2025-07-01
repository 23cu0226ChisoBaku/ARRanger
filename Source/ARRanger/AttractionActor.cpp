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

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponent��ǉ����ABoxComponent��RootComponent��Attach����
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AAttractionActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlap��Bind����
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
		// �v���C���[�Ƃ̕����Ƌ������擾
		FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
		float Distance = Direction.Size();
		UE_LOG(LogTemp, Warning, TEXT("PushOffset size: %f"), Direction.Size());

		FVector DirectionNorm = Direction / Distance;

		// ���C�͂𔭂��ăv���C���[����������
		FVector pushForce = DirectionNorm * constProp * magneticValue * magneticValue / (Distance * Distance);
		playerCharacter->AddActorWorldOffset(-pushForce, true);
	}

}

void AAttractionActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �ڐG����Actor��AInsekiCharacter�����肷��
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		// �����Ă����t���O���I����
		onStayFlag = true;
	}
}

void AAttractionActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �ڐG����Actor��AInsekiCharacter�����肷��
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("tasukattaaaaaa"));
		// �����Ă����t���O���I�t��
		onStayFlag = false;
		repulsionFlag = false;
	}
}