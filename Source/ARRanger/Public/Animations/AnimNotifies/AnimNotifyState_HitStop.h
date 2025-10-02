//*************************************************
// ヒットストップの処理を行うアニメーション通知ステート
//*************************************************

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_HitStop.generated.h"

#define UE_API ARRANGER_API

UCLASS(editinlinenew, MinimalAPI, meta = (DisplayName = "HitStop") )
class UAnimNotifyState_HitStop : public UAnimNotifyState
{
	GENERATED_BODY()

protected:	
	UE_API virtual void NotifyBegin(USkeletalMeshComponent * meshComp, UAnimSequenceBase * animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
  	UE_API virtual void NotifyEnd(USkeletalMeshComponent * meshComp, UAnimSequenceBase * animation, const FAnimNotifyEventReference& eventReference) override;
  	UE_API FString GetNotifyName_Implementation() const override;

private:
  	UPROPERTY(EditAnywhere, Category = "HitStop", meta = (DisplayName = "HitStop", ClampMin = 0.1))
	float m_HitStopMotionSpeed;		/*ヒットストップ時のモーション速度*/
	UPROPERTY()
	float m_OriginalMotionSpeed;	/*通常時のモーション速度*/
};

#undef UE_API