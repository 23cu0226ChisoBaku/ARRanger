/**
 * @file ARPlayerInputBuffer.h
 * @author MAI ZHICONG
 * @brief プレイヤー入力バッファクラス
 * 
 */

#pragma once

#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"

#include "ARPlayerInputBuffer.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class AARRangerPlayerController;
class UARAbilitySystemComponent;
namespace ARRanger
{
  namespace Input
  {
    class FARInputBufferState;
  } // namespace ARRanger::Input
} // namespace ARRanger


UCLASS(Blueprintable, BlueprintType)
class UARPlayerInputBuffer : public UObject
{
	GENERATED_BODY()

public:

  UE_API UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief 入力バッファを初期化
   * @param InPlayerController プレイヤーコントローラー
   */
  UE_API void Initialize(AARRangerPlayerController* InPlayerController);

  /**
   * @brief 入力バッファ初期化を解除する
   */
  UE_API void Uninitialize();

  /**
   * @brief 入力バッファを評価する
   * @param InPlayerControllerプレイヤーコントローラー
   * @param DeltaTime 
   * @param bGamePaused
   */
  UE_API void EvaluateBuffer(const float DeltaTime, const bool bGamePaused);

  /**
   * @brief 入力バッファを全部消す
   */
  UE_API void ClearAllInputs();

  /**
   * @brief 入力ボタンが押された時に入力Tagを処理する
   * @param InInputTag 入力アクションにバインドしたTag
   * @param bOverrideInputState 入力ステートを別のステート上書きするか
   */
  UE_API void HandleInputTagPressed(const FGameplayTag& InInputTag, bool bOverrideInputState);

  /**
   * @brief 入力ボタンが離れた時に入力Tagを処理する
   * @param InInputTag 入力アクションにバインドしたTag
   * @param bOverrideInputState 入力ステートを別のステート上書きするか
   */
  UE_API void HandleInputTagReleased(const FGameplayTag& InInputTag, bool bOverrideInputState);

private:

  /**
   * @brief 入力バッファを消耗する
   * @param InInputTag 消耗する入力ステートのTag
   */
  void ConsumeBuffer(const FGameplayTag& InInputTag);

  /**
   * @brief 無効になった入力ステートを削除する
   * @param InPlayerController プレイヤーコントローラー
   */
  void RemoveExpiredInputState();

  /**
   * @brief TryActivateAbilityが処理されたときのコールバック
   * @param InAbilitySystemComponent アビリティシステムコンポーネント
   * @param InAbilityAssetTags       処理したアビリティのタグコンテナ
   * @param bResult                  TryActivateAbilityの実行結果
   */
  UFUNCTION()
  void OnTryActivateAbilityHandled(UARAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InAbilityAssetTags, bool bResult);

private:

  /**前の入力状態の維持時間 */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  float InputKeepTime;

  /**ゲームが止まったら評価するかを表すフラグ */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  bool bEvaluateIfGamePaused;

  /**プレイヤーコントローラー */
  UPROPERTY()
  TObjectPtr<AARRangerPlayerController> PlayerController;

  /**全ての入力バッファステートを管理する配列 */
  TArray< TPimplPtr< ARRanger::Input::FARInputBufferState > > m_inputStates;

};

#undef UE_API