//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#pragma once

#include "Interface/IARObjectInterface.h"
#include "ARObjectBase.generated.h"

// 前方宣言
class UARObjectComponent;

UCLASS()
class AARObjectBase : public AActor, public IARObjectInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ARObjectComponent)
	UARObjectComponent* _pARObjectComponent;

public:
	virtual void OnAttraction() override;
	virtual void OnRepulsion() override;

	UFUNCTION(BlueprintCallable, Category = "Attractable")
	void SetNewARType(EARType newType) const;

	UFUNCTION(BlueprintCallable, Category = "Attractable")
	EARType GetCurrentARType() const;

	/*
	* コンストラクタ
	*/
	AARObjectBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EARType CurrentARType;
};