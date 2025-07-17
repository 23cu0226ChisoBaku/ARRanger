//*************************************************
// プレイヤーの待機状態の処理(挙動)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "Interface/IJumpInterface.h"
#include "PlayerJumpStateInstance.generated.h"

/**
 * Player Jump Instance
 */
UCLASS()
class ARRANGER_API UPlayerJumpStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

	UFUNCTION(BlueprintCallable, Category = "IJump")
	void RequestJump(UObject* Target);

	/*
	* IJumpInterface 
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	void IJump();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	bool ICanJump() const;


};
