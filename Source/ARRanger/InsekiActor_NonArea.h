#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IARMagnetizableInterface.h"

#include "InsekiActor_NonArea.generated.h"

class UCapsuleComponent;

UCLASS()
class ARRANGER_API AInsekiActor_NonArea : public AActor,
										  public IARMagnetizableInterface
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

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnAttraction() { }
	virtual void OnRepulsion() { }
	virtual AActor* GetActor() { return this; }

	// OnHitä÷êîÇêÈåæ
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};