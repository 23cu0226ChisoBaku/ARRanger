//*************************************************
// 引力フックショット(仮)の挙動を行うコンポーネント
//*************************************************

#pragma once

#include "Components/ActorComponent.h"
#include "HookshotComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHookshotComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:	
	UHookshotComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

    void HookshotAction(AActor* targetActor);
    FVector CalculationDirection();


private:

    UPROPERTY()
    TObjectPtr<AActor> m_TargetActor;
    UPROPERTY()
    AActor* m_OwnerActor;
    UPROPERTY()
    bool m_CanHookshot;
    UPROPERTY()
    bool m_HookshotAction;
};
