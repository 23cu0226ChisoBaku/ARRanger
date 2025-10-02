//*************************************************
// ヒットストップの処理を行うアニメーション通知ステート
//*************************************************

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_HitStop.generated.h"

#define UE_API ARRANGER_API

UCLASS(editinlinenew, )
class UAnimNotifyState_HitStop : public UAnimNotifyState
{
	GENERATED_BODY()
public:	
	UE_API virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
  	UE_API virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
  	UE_API FString GetNotifyName_Implementation() const override;

  	UPROPERTY(EditAnywhere, Category = "" , meta = (DisplayName = "Play Speed", ClampMin = 0.1))
	float m_MotionSpeed;		/*モーション速度*/
};

#undef UE_API