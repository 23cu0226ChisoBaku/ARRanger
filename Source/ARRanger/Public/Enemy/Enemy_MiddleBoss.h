
#pragma once

#include "Enemy/Enemy_Zako.h"

#include "Enemy_MiddleBoss.generated.h"

#define UE_API ARRANGER_API

enum class EAttackType : uint8;

UCLASS()
class AEnemy_MiddleBoss : public AEnemy_Zako
{
	GENERATED_BODY()

public:

  UE_API AEnemy_MiddleBoss();

  class UE_API FPreAttackTask
  {
    public:
      FPreAttackTask(AEnemy_MiddleBoss* InSourceBoss, EAttackType InTaskType)
        : SourceBoss{InSourceBoss}
        , TaskType{InTaskType}
      { }

      virtual ~FPreAttackTask() = default;
      virtual void UpdateTask(float DeltaTime) = 0;
      uint8 bIsFinished : 1 = false;
      
    protected:
      TWeakObjectPtr<AEnemy_MiddleBoss> SourceBoss = nullptr;
      EAttackType TaskType;
  };

  DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreAttackTaskFinishedDelegate, EAttackType);
  FOnPreAttackTaskFinishedDelegate OnPreAttackTaskFinishedEvent;

  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinishedDelegate);
  UPROPERTY(BlueprintAssignable)
  FOnAttackFinishedDelegate OnAttackFinished;

  UE_API virtual void OnAttackPerformed(EAttackType InAttackType);

  UE_API virtual void OnPreAttackTaskFinished(EAttackType InAttackType);

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Enemy")
  UE_API void K2_OnAttackPerformed(EAttackType InAttackType);

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Enemy")
  UE_API void K2_OnPreAttackTaskFinished(EAttackType InAttackType);

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Enemy")
  UE_API void K2_OnAttackFinished();

  UE_API void UpdatePreAttack(float DeltaTime);

  UE_API void SetTargetActor(AActor* InTargetActor);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float CurrentSpeed;

  UPROPERTY(EditDefaultsOnly, Category = "Enemy|Attack")
  float PunchRange;

  UPROPERTY(EditDefaultsOnly, Category = "Enemy|Attack")
  float JumpMoveSpeed;

private:

  UPROPERTY()
  TObjectPtr<AActor> TargetActor;

  TUniquePtr<FPreAttackTask> Task; 
};

#undef UE_API
