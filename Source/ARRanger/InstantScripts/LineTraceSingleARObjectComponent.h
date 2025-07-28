// ARObjectをラインとレースで取得するコンポーネント(即席スクリプト)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/IARObjectInterface.h"
#include "Interface/IARTypeInterface.h"
#include "LineTraceSingleARObjectComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API ULineTraceSingleARObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULineTraceSingleARObjectComponent();

	// ライントレース用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float LineTraceLength;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// レイキャストを行い、インターフェースを実装しているオブジェクトを取得
	UFUNCTION(BlueprintCallable, Category = "Raycast")
	void PerformRaycast();
};
