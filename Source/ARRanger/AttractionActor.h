#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttractionActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class ARRANGER_API AAttractionActor : public AActor
{
	GENERATED_BODY()

public:
	AAttractionActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float constProp;

	UPROPERTY(EditAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	float magneticValue;

	bool onStayFlag;

	bool repulsionFlag;

	AActor* playerCharacter;

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void Tick(float DeltaTime) override;

};