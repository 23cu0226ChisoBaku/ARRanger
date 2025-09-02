//*************************************************
// 空き缶インスタンス
//*************************************************
#pragma once

#include "GameFramework/Actor.h"
#include "EmptyCan.generated.h"

UCLASS()
class ARRANGER_API AEmptyCan : public AActor
{
	GENERATED_BODY()

public:	
	AEmptyCan();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

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
	float GetFlyForceByDamage();

	UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;
};
