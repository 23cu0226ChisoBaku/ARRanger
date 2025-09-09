// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**前方宣言 */
class UObject;
class AController;
class UMStateMachineComponent;
struct FGameplayTag;

#define UE_API MSTATEMACHINE_API

/**
 * ステートコンテキスト初期化パラメータ
 */
struct FMStateContextInitializeParameters
{
  /**
   * ステートOwner
   */
  TObjectPtr<UObject> Owner;

  /**
   * ステートOwnerのController
   * 
   * Maybe nullptr
   */
  TObjectPtr<AController> OwnerController;

  /**
   * このステートコンテキストを管理するステートマシンコンポーネント
   */
  TObjectPtr<const UMStateMachineComponent> StateMachineComponent;
};


/**
 * ステートコンテキスト
 */
class FMStateContext
{
  friend class UMStateMachineComponent;

  public:
    UE_API FMStateContext();
    UE_API virtual ~FMStateContext();

  public:

    UObject* GetOwner() const { return m_weakOwner.Get(); }
    AController* GetOwnerController() const { return m_weakController.Get();} 
    const UMStateMachineComponent* GetStateMachineComponent() const { return m_weakStateMachineComponent.Get(); } 
    bool IsValid() const { return m_bIsValid; }
    
    UE_API UWorld* GetWorld() const;
    
    /**
     * @brief 今のステートを抜けれるTransition tagを取得
     * 
     * @param OutTags Transition tag
     * 
     * @return 取得できたタグの数
     */
    UE_API int32 GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const;
  
  private:

    /**
     * @brief コンテキストを初期化
     * 
     * @param InitParams 初期化パラメータ 
     */
    void InitializeContext(const FMStateContextInitializeParameters& InitParams);

  protected:

    /**
     * @brief コンテキストを初期化した後のコールバック
     * 
     * @param InitParams 初期化パラメータ 
     */
    UE_API virtual void OnInitializeContext(const FMStateContextInitializeParameters& InitParams) {}

  private:
    /**Owner弱参照 */
    TWeakObjectPtr<UObject> m_weakOwner;

    /**Owner Controller弱参照 */
    TWeakObjectPtr<AController> m_weakController;

    /**ステートマシンコンポネント弱参照 */
    TWeakObjectPtr<const UMStateMachineComponent> m_weakStateMachineComponent;

    bool m_bIsValid : 1;
};

#undef UE_API