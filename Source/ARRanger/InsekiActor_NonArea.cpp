#include "InsekiActor_NonArea.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AInsekiActor_NonArea::AInsekiActor_NonArea()
	: InsekiActorMesh{ nullptr }
	, Capsule{ nullptr }
	, playerCharacter{ nullptr }
{
	PrimaryActorTick.bCanEverTick = false;

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// CapsuleComponent��ǉ����ACapsuleComponent��RootComponent��Attach����
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	Capsule->SetupAttachment(RootComponent);
}

void AInsekiActor_NonArea::BeginPlay()
{
	//Super::BeginPlay();

	//// OnHit��Bind����
	//Capsule->OnComponentHit.AddDynamic(this, &AInsekiActor_NonArea::OnHit);

	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);

	//if (FoundActors.Num() > 0 && FoundActors[0] != nullptr)
	//{
	//	playerCharacter = FoundActors[0];
	//}
	//else
	//{
	//	playerCharacter = nullptr;
	//	UE_LOG(LogTemp, Error, TEXT("AInsekiActor_NonArea: No valid actor found with tag 'Player'"));
	//}

	SetMagnetismType(EARMagnetismType::Repulsion);
}

void AInsekiActor_NonArea::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

	//if (!playerCharacter)
	//{
	//	TArray<AActor*> FoundActors;
	//	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);
	//	if (FoundActors.Num() > 0)
	//	{
	//		playerCharacter = FoundActors[0];
	//		UE_LOG(LogTemp, Warning, TEXT("Found PlayerCharacter later in Tick"));
	//	}
	//}
}

void AInsekiActor_NonArea::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//// �v���C���[�̌��݂̃��[�h���m�F�i���� or �˗́j
	//// �擾�ł��Ȃ���Ώ������Ȃ�
	//AARRangerCharacter* player = Cast<AARRangerCharacter>(playerCharacter);
	//if (!player)
	//{
	//	return;
	//}

	//// �˗͏�Ԃ̂Ƃ��̂ݐ�����΂�
	//if (player->GetCurrentARType() == EARMagnetismType::Attraction)
	//{
	//	// ���͏�Ԃ̏ꍇ�͂Ȃɂ����Ȃ�
	//	UE_LOG(LogTemp, Warning, TEXT("Player is in Attraction mode: No knockback"));
	//	return;
	//}
	//else if (player->GetCurrentARType() == EARMagnetismType::Repulsion)
	//{
	//	// �ڐG�������擾
	//	FVector ImpactDirection = playerCharacter->GetActorLocation() - GetActorLocation();
	//	ImpactDirection.Normalize();

	//	// �^�ォ�痈�Ă邩�ǂ�������(Z���������ŏ\���傫��)
	//	// �^�ォ�痎���Ă����ꍇ
	//	if (ImpactDirection.Z > 0.7f)
	//	{
	//		// �v���C���[�̐i�s�����i���x�x�N�g���j���擾
	//		FVector Velocity = player->GetVelocity();

	//		// ���͂����邩�ǂ����`�F�b�N�i�ړ����Ă��邩�j
	//		if (Velocity.SizeSquared() > KINDA_SMALL_NUMBER)
	//		{
	//			// �O�����i�i�s�����j�Ɍ������ăW�����v
	//			FVector ForwardDir = Velocity.GetSafeNormal();

	//			// ��荂���������I�ɔ�΂�
	//			FVector LaunchVelocity = ForwardDir * 1200.f + FVector(0.f, 0.f, 800.f);
	//			player->LaunchCharacter(LaunchVelocity, true, true);

	//			UE_LOG(LogTemp, Warning, TEXT("Repulsion forward launch: %s"), *LaunchVelocity.ToString());
	//		}
	//		else
	//		{
	//			// �����ĂȂ���ΐ^��ɔ����W�����v
	//			FVector LaunchVelocity = FVector(0.f, 0.f, 1400.f);
	//			player->LaunchCharacter(LaunchVelocity, true, true);

	//			UE_LOG(LogTemp, Warning, TEXT("Repulsion vertical launch only (no input)"));
	//		}
	//	}
	//	else
	//	{
	//		// �����瓖�������ꍇ�͐�����΂�(Z�͏��������AXY�����ɔ�΂�)
	//		FVector HorizontalDir = ImpactDirection;
	//		HorizontalDir.Z = 0.f;
	//		HorizontalDir.Normalize();

	//		FVector KnockbackVelocity = HorizontalDir * 800.f + FVector(0.f, 0.f, 200.f);
	//		player->LaunchCharacter(KnockbackVelocity, true, false);
	//	}
	//}
}