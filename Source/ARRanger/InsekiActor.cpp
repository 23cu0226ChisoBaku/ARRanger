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

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// SphereComponent��ǉ����ASphereComponent��RootComponent��Attach����
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AInsekiActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlap��Bind����
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AInsekiActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AInsekiActor::OnSphereEndOverlap);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);
	playerCharacter = FoundActors[0];

	// �v���C���[��RootComponent���擾
	PlayerRoot = Cast<UPrimitiveComponent>(playerCharacter->GetRootComponent());
}

void AInsekiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (onStayFlag == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("huttobeeeeeee"));

		// �v���C���[�Ƃ̕����Ƌ������擾
		FVector Direction = playerCharacter->GetActorLocation() - GetActorLocation();
		float Distance = Direction.Size();

		// �x�N�g����������
		FVector DirectionNorm = Direction / Distance;

		UE_LOG(LogTemp, Warning, TEXT("PushOffset size: %f"), Direction.Size());
		//repulsionFlag = true;

		if (Distance <= 200.f)
		{
			repulsionFlag = true;
		}
		if (repulsionFlag == true)
		{
			// ���C�͂𔭂��ăv���C���[�������o��
			FVector pushForce = DirectionNorm * constProp * magneticValue * magneticValue / (Distance * Distance);

			// �f�o�b�O�m�F�p
			UE_LOG(LogTemp, Warning, TEXT("pushForce Value Is : %f"), pushForce.Size());

			playerCharacter->AddActorWorldOffset(pushForce, false);

			/*// ACharacter�ɃL���X�g���ē�����
			ACharacter* Char = Cast<ACharacter>(playerCharacter);
			Char->LaunchCharacter(pushForce, false, false);*/
		}
	}
}

void AInsekiActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �ڐG����Actor��AInsekiCharacter�����肷��
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("gomikasuuuuuuu"));
		RootPrimitive = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
		// �����Ă����t���O���I����
		onStayFlag = true;
	}
}

void AInsekiActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("kutabareeeeee"));
	// �ڐG����Actor��AInsekiCharacter�����肷��
	if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(OtherActor))
	{
		RootPrimitive = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("shinebokeeeeeee"));
		// �����Ă����t���O���I�t��
		onStayFlag = false;
		repulsionFlag = false;
	}
}