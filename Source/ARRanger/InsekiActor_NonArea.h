#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InsekiActor_NonArea.generated.h"

class UCapsuleComponent;

UCLASS()
class ARRANGER_API AInsekiActor_NonArea : public AActor
{
	GENERATED_BODY()

public:
	AInsekiActor_NonArea();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> Capsule;

	TObjectPtr<AActor> playerCharacter;

	UPROPERTY()
	UPrimitiveComponent* RootPrimitive;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float basePower;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float minValue;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float maxValue;

	// LaunchCharacterで使う予定の速度を保存する変数
	FVector PendingLaunchVelocity;

	// 最後にぶつかった時間(初期値は十分過去の時間のものにする)
	float lastHitTime;

	// 一定間隔で処理をするためのクールタイム
	float hitCoolTime;

	// ぶつかった時に遅延でLaunchCharacterを呼ぶためのタイマー
	FTimerHandle LaunchTimerHandle;

	FTimerHandle ResetMovementTimerHandle;

private:
	// 遅延呼び出し用関数（OnHit内から呼ばれる）
	void PerformLaunch();

	void RestoreMovementMode();

public:
	virtual void Tick(float DeltaTime) override;

	// OnHit関数を宣言
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};