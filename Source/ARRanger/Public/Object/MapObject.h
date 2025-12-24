#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapObject.generated.h"

UCLASS()
class ARRANGER_API AMapObject : public AActor
{
	GENERATED_BODY()
	
protected:
	// オブジェクトの引き寄せ開始までにかかる時間(オブジェクトの耐久値)
	float durabilityTime;

	// オブジェクトの大きさ分け(三段階 1～3の順に大きくなる)
	int sizeValue;

public:	
	AMapObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};