#pragma once

#include "GameFramework/Actor.h"

#include "Physics/IARPhysicsSystemHost.h"
#include "IARMagnetizableInterface.h"

#include "AttractionActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class ARRANGER_API AAttractionActor : public AActor,
                                      public IARPhysicsSystemHost,
                                      public IARMagnetizableInterface
{
	GENERATED_BODY()

public:
	AAttractionActor();

  virtual void OnAttraction() override {}

	virtual void OnRepulsion() override {}

	virtual AActor* GetActor() override { return this; }
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

  

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};