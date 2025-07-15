#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InsekiActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class AInsekiActor : public AActor
{
	GENERATED_BODY()

public:
	AInsekiActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	// é•ãCó 
	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float magneticValue;

	// î‰ó·íËêî
	float constProp;

	bool onStayFlag;

	bool repulsionFlag;

	bool isLaunchedFlag;

	AActor* playerCharacter;

	UPROPERTY()
	UPrimitiveComponent* RootPrimitive;

	UPROPERTY()
	UPrimitiveComponent* PlayerRoot;

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void Tick(float DeltaTime) override;
};