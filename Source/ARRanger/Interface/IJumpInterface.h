//*************************************************
// �W�����v�����邽�߂̃C���^�[�t�F�[�X
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IJumpInterface.generated.h"

/*
* BP����p���\�ȃC���^�[�t�F�[�X(���t�@�����X�p)
*/
UINTERFACE(MinimalAPI)
class UJumpInterface : public UInterface
{
	GENERATED_BODY()
};

class IJumpInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	void IJump();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	bool ICanJump() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	void IJump();

};