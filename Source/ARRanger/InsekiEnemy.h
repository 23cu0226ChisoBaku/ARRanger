#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "InsekiEnemy.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ARRANGER_API AInsekiEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AInsekiEnemy();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InsekiActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inseki, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Box;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};