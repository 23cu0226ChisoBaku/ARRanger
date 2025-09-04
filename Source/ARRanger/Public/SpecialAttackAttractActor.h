//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************
#pragma once

#include "GameFramework/Actor.h"
#include "ISpecialAttractInterface.h"
#include "SpecialAttackAttractActor.generated.h"

UCLASS()
class ARRANGER_API ASpecialAttackAttractActor : public AActor,
                                                public ISpecialAttractInterface
{
	GENERATED_BODY()	
public:	
	ASpecialAttackAttractActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnStartSpecialAttact();

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnEndSpecialttact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true", MustImplement = "ARAttackable"))
	TSet<TSubclassOf<AActor>> m_AttractionClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	TSet<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> m_previousDetectedActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	float m_DetectionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	float m_AttractSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater", meta = (AllowPrivateAccess = "true"))
	float m_RotationSpeed;
};