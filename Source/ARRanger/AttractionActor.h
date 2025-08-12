#pragma once

#include "GameFramework/Actor.h"

#include "Physics/IARPhysicsSystemHost.h"
#include "IARMagnetizableInterface.h"

#include "AttractionActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class ARRANGER_API AAttractionActor : public AActor,
                                      public IARMagnetizableInterface
{
	GENERATED_BODY()

public:
	AAttractionActor();

  virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) override {}

	virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override {}

	virtual AActor* GetActor() override { return this; }
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;
};