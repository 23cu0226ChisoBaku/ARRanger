#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ARRANGER_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Box;

protected:
	// 最大HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int maxHP;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 現在のHP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	int currentHP;

	// 死亡フラグ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool isDead;

	// ダメージを受ける関数
	UFUNCTION()
	void ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop);
};