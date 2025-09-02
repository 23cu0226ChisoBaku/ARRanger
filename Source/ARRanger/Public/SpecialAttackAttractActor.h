//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************
#pragma once

#include "GameFramework/Actor.h"
#include "SpecialAttackAttractActor.generated.h"

UCLASS()
class ARRANGER_API ASpecialAttackAttractActor : public AActor
{
	GENERATED_BODY()	
public:	
	ASpecialAttackAttractActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> m_TargetAttractionActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	float m_AttractForce;
};
