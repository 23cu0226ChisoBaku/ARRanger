// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARAttackable.generated.h"

#define UE_API ARRANGER_API

namespace ARRanger
{

namespace Battle
{
  enum struct EARAttackResult : uint8
  {
    Success,  // 成功
    Inmune,   // 無敵
    Rebound,  // 反発
  };

  /**
   * @brief 攻撃結果構造体
   */
  struct FARAttackResult
  {
    EARAttackResult Result = EARAttackResult::Inmune;
  };
  
  /**
   * @brief ダメージ結果構造体
   */
  struct FARDamageResult
  {
    TObjectPtr<AActor> Instigator;

    float FinalDamage = 0.0f;

    FVector FinalLaunchDirection = FVector::ZeroVector;

    FVector ImpactLocation = FVector::ZeroVector;
  };
}  

}


/**
 * @brief 攻撃パラメータ構造体
 */
USTRUCT(BlueprintType)
struct FARAttackParameters
{
  GENERATED_BODY()
  
  /**扇動者 */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  TObjectPtr<AActor> Instigator;
  
  /**ダメージ */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  float Damage;
  
  /**飛ばす方向(単位ベクトル) */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  FVector LaunchDirection;

  /**攻撃の与える座標 */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  FVector ImpactLocation;
  
  /**
   * 攻撃者のアクターを使用するか
   * falseかつInstigatorが有効だったらInstigatorを使い、それ以外は攻撃者のActorを使う
   */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  bool bUseAttackerActor;

  ARRANGER_API FARAttackParameters();

  /**空白攻撃パラメータ */
  ARRANGER_API static const FARAttackParameters BlankAttackParams;
  static const FARAttackParameters& GetBlank() { return BlankAttackParams; }
};

/**Forward declaration */
class IARAttackerInterface;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARAttackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IARAttackable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  /**
   * @brief                 物を攻撃する
   * @param Attacker        攻撃者
   * @param InAttackParams  攻撃パラメータ
   * @return 成功だったらtrue, それ以外はfalseを返す
   */
  UE_API bool AttackTarget(IARAttackerInterface* Attacker, FARAttackParameters InAttackParams = FARAttackParameters::GetBlank());

  /**
   * @brief   アクターを返す
   * @return  AActor
   */
  UE_API virtual AActor* Attackable_GetActor() { return ::Cast<AActor>(_getUObject()); }

  /**
   * @brief   攻撃できるか
   * @return  攻撃できたらtrue, それ以外はfalse
   */
  UE_API virtual bool CanAttack() { return true; }

protected:

  /**
   * @brief 攻撃を受ける前に呼び出されるコールバック
   */
  UE_API virtual void OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult) { }

  /**
   * @brief 攻撃を受けた後に呼び出されるコールバック
   */
  UE_API virtual void OnPostAttacked(const FARAttackParameters& InAttackParams) { }

  /**
   * @brief ダメージ計算を済んだ時に呼び出されるコールバック
   */
  UE_API virtual void OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult) { }
};

#undef UE_API