#include "Enemy.h"

#include "ARRangerGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
	: InsekiActorMesh(nullptr)
	, Box(nullptr)
	, maxHP(100)
	, currentHP(maxHP)
	, isDead(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponent��ǉ����ABoxComponent��RootComponent��Attach����
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop)
{
	currentHP -= DamageAmount;

	if (currentHP <= 0)
	{
		// ���S�t���O���グ��
		isDead = true;

		// �Ō�̈ꌂ�I����������΂�
		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(GetRootComponent()))
		{
			if (Comp->IsSimulatingPhysics())
			{
				Comp->AddImpulse(LaunchDirection * 1500.f, NAME_None, true);
			}
		}

		// �G�t�F�N�g��SE�������ōĐ��I


		// GameMode�ɒʒm
		if (AARRangerGameMode* GM = Cast<AARRangerGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast Success!"));
			GM->OnEnemyKilled();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cast Failed! GameMode is %s"), *UGameplayStatics::GetGameMode(this)->GetClass()->GetName());
		}
		// ������Ƒ҂��Ă������
		SetLifeSpan(1.0f);
	}
	else
	{
		// �r���̍U���B�y��������΂�����
		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(GetRootComponent()))
		{
			if (Comp->IsSimulatingPhysics())
			{
				Comp->AddImpulse(LaunchDirection * 600.f, NAME_None, true);
			}
		}
	}

	// �q�b�g�X�g�b�v�F�Ƃǂ߂̈ꌂ�����L����
	if (bEnableHitStop)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, []()
			{
				UGameplayStatics::SetGlobalTimeDilation(GWorld, 1.0f);
			}, 0.03f, false);
	}
}