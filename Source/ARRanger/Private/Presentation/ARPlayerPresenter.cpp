// Fill out your copyright notice in the Description page of Project Settings.

#include "Presentation/ARPlayerPresenter.h"

#include "Character/ARRangerCharacter.h"
#include "Character/ARHealthComponent.h"
#include "BattleSystem/IARAttackable.h"

namespace
{
  // Input max value(scalar)
  constexpr float MAX_INPUT_VALUE = 1.0f;
}

void UARPlayerPresenter::Initialize(AARRangerCharacter* InViewCharacter)
{
  check(InViewCharacter != nullptr);

  if (View == InViewCharacter)
  {
    return;
  }

  if (View != nullptr)
  {
    Deinitialize();
  }

  View = InViewCharacter;
  if (View != nullptr)
  {
    View->OnBattleResultAccepted.AddUObject(this, &ThisClass::HandleBattleResult);
    View->OnBattleStateChanged.AddUObject(this, &ThisClass::HandleBattleStateChange);
    Model.HealthComponent = static_cast<UARHealthComponent*>(View->GetComponentByClass(UARHealthComponent::StaticClass()));
  }
}

void UARPlayerPresenter::Deinitialize()
{
  if (View == nullptr)
  {
    return;
  }

  View->OnBattleResultAccepted.RemoveAll(this);
  Model.HealthComponent = nullptr;
}

void UARPlayerPresenter::HandleMoveInput(double InRight, double InForward, double InDeadZone)
{
  if (View == nullptr)
  {
    return;
  }

  // TODO Switch to Move condition
  const bool bCanMove = false;
  if (!bCanMove)
  {
    return;
  }

  // 入力値の絶対値をチェックしてデッドゾーン以下は0に
  // Modified By MAI
  const float radiusSquared = FMath::Square(InForward) + FMath::Square(InRight);
  const float moveDeadZoneSquared = FMath::Square(FMath::Max(0.0, InDeadZone));

  // デッドゾーン以下
  if (radiusSquared <= moveDeadZoneSquared)
  {
    return;
  }
  
  const float realMinInput = FMath::Min(MinInput, MAX_INPUT_VALUE);
  // インプット閾値レベル
  const TArray<float> inputThresholdLevel{ 
                        realMinInput,       // LV1  : 最小入力値
                        MAX_INPUT_VALUE,    // LVMax: 最大入力値
                      };

  for (int32 idx = 0; idx < inputThresholdLevel.Num(); ++idx)
  {
    // インプット閾値まで補正する
    const float inputLevelValue = inputThresholdLevel[idx];
    const float inputLevelValueSquared = FMath::Square(inputLevelValue);
    if (radiusSquared < inputLevelValueSquared)
    {
      const float inputModifier = inputLevelValue / FMath::Sqrt(radiusSquared);
      Forward *= inputModifier;
      Right   *= inputModifier;

      break;
    }   
  }

  View->DoMove(InRight, InForward);


}

void UARPlayerPresenter::HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  // check AffectedCharacter is same as View character
  check(InAffectedCharacter == View);

  // Value of damage is positive. Make it negative
  const float HPChangeValue = -InDamageResult.FinalDamage;
  bool bIsDead = false;

  if (Model.HealthComponent != nullptr)
  {
    Model.HealthComponent->HandleHealthChange(InDamageResult.Instigator, HPChangeValue);

    if (Model.HealthComponent->GetHealth() > 0.0f)
    {
      FVector launchDirNorm = InDamageResult.FinalLaunchDirection;
      launchDirNorm.Z = 0.0;
      launchDirNorm.Normalize();
      const double launchPowerFactor = 1.0;

      if (View != nullptr)
      {
        const double finalLaunchPower = Model.LaunchPower * launchPowerFactor;
        View->LaunchCharacter_Ext(launchDirNorm, finalLaunchPower);
      }
    }
    else
    {
      bIsDead = true;
    }
  }

  if (View != nullptr)
  {
    View->OnHealthChanged(InDamageResult.Instigator, HPChangeValue, bIsDead);
  }

}

void UARPlayerPresenter::HandleBattleStateChange(bool bIsInBattle)
{
  // TODO We should not let auto generation system in HealthComponent
  if (Model.HealthComponent != nullptr)
  {
    Model.HealthComponent->SetAutoRegenerationEnable(!bIsInBattle);
  }
}