//*************************************************
// 空き缶インスタンス
//*************************************************
#pragma once

#include "GameFramework/Actor.h"

#include "BattleSystem/IARAttackable.h"

#include "EmptyCan.generated.h"

UCLASS()
class AEmptyCan : public AActor,
                  public IARAttackable
{
	GENERATED_BODY()

public:	
	ARRANGER_API AEmptyCan();
	ARRANGER_API virtual void BeginPlay() override;
	ARRANGER_API virtual void Tick(float DeltaTime) override;


/**Start IARAttackable Interface */
public:
  ARRANGER_API virtual AActor* Attackable_GetActor() override { return this; }

protected:
  ARRANGER_API virtual void OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult) override;
  ARRANGER_API virtual void OnPostAttacked(const FARAttackParameters& InAttackParams) override;
/**End IARAttackable Interface */

private:

	/**
	 * @brief 
	 */

	/**
	 * @brief ダメージ量に適した飛ぶ力を取得する
	 * 
	 * @param 飛ぶ力
	 */
	UFUNCTION()
	ARRANGER_API float GetFlyForceByDamage();

	UPROPERTY(EditDefaultsOnly)
  TObjectPtr<UStaticMeshComponent> MeshComponent;
};
