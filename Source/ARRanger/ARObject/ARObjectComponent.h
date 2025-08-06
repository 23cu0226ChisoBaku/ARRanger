//*************************************************
// 斥力引力の挙動を処理するコンポーネント
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/IARTypeInterface.h"
#include "ARObjectComponent.generated.h"

/*
*  UARObjectComponentの処理
*  BPから追加可能
*/
UCLASS(ClassGroup = (ARObject), meta = (BlueprintSpawnableComponent))
class UARObjectComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

	UPROPERTY()
	EARType _CurrentARType = EARType::None;	// 現在の状態

public:
	UFUNCTION(BlueprintCallable)
	void SetARType(EARType newType);  // 状態を設定する関数

	UFUNCTION(BlueprintCallable, Category = "Attractable")
	EARType GetCurrentARType() const { return _CurrentARType; };

	/*
	* コンストラクタ
	*/
	UARObjectComponent();
};