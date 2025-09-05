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
    virtual void BeginDestroy() override;

	UFUNCTION()
    void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

private:

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnStartSpecialAttact();

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnEndSpecialttact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Target", meta = (AllowPrivateAccess = "true", MustImplement = "ARAttackable"))
	TSet<TSubclassOf<AActor>> m_AttractionClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Target", meta = (AllowPrivateAccess = "true"))
	TSet<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract", meta = (AllowPrivateAccess = "true"))
	float m_DetectionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract ", meta = (AllowPrivateAccess = "true"))
	float m_AttractSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract", meta = (AllowPrivateAccess = "true"))
	float m_RotationSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Explosion", meta = (AllowPrivateAccess = "true"))
	float m_ExplosionPower;

    UPROPERTY()
	TArray<AActor*> m_previousDetectedActors;
};