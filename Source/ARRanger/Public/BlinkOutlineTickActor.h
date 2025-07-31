//*************************************************
// アウトラインの点滅処理を毎フレーム呼び出すアクター
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "BlinkOutlineTickActor.generated.h"

class ARRANGER_API ABlinkOutlineTickActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlinkOutlineTickActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
