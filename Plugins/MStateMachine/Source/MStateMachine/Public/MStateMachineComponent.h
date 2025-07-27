// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateMachineComponent.generated.h"

/**前方宣言 */
class UMStateInstance;
class UMStateDefinition;
class FMStateContext;

/**
 * ステートを抜ける理由列挙
 */
enum class EStateExitReason
{
  Transition,
  Uninitialize
};


/**
 * ステートハンドル
 */
USTRUCT(BlueprintType)
struct FMStateHandle
{
  GENERATED_BODY()
  
  friend class UMStateMachineComponent;

  MSTATEMACHINE_API FMStateHandle();
  MSTATEMACHINE_API FMStateHandle(UMStateInstance* State, UActorComponent* OwnerComp, const FGameplayTag& StateTag);

  /**
   * @brief Handle有効化チェック
   * 
   * @return インスタンス、コンポーネント、タグがすべて有効値だったらtrue、それ以外はfalseを返す
   */
  MSTATEMACHINE_API bool IsValid() const;

  /**
   * @brief ステートタグを取得
   * 
   * @return タグが無効だったらFGameplayTag::EmptyTagを返す
   */
  MSTATEMACHINE_API FGameplayTag GetStateTag() const;
  
private:
  /**ステートインスタンス弱参照 */

  TWeakObjectPtr<UMStateInstance> m_state;
  
  /**Ownerコンポーネント弱参照 */
  TWeakObjectPtr<UActorComponent> m_ownerComp;
  
  /**ステートタグ */
  FGameplayTag m_stateTag;
};


/**
 * ステートインスタンスリストオブジェクト
 */
USTRUCT()
struct FMStateMachineStateListEntry
{
  GENERATED_BODY()

  friend struct FMStateMachineStateList;
  friend class UMStateMachineComponent;

  FMStateMachineStateListEntry();

private:

  /**ステートインスタンス */
  UPROPERTY()
  TObjectPtr<UMStateInstance> State;

  /**ステート定義アセット */
  UPROPERTY()
  TObjectPtr<const UMStateDefinition> StateDefinition;

};


/**
 * ステートインスタンスリスト
 */
USTRUCT()
struct FMStateMachineStateList
{
  GENERATED_BODY()
  
  friend class UMStateMachineComponent;

public:
  FMStateMachineStateList();
  explicit FMStateMachineStateList(UMStateMachineComponent* OwnerComp);

  /**
   * @brief 新しいステートエントリーを追加
   * 
   * @param DefinitionClass 初期化するのに使うUMStateDefinitionのUClass及び子クラスのUClass
   * 
   * @return 追加成功したら有効なHandleを返し、それ以外の場合は無効なHandle
   */
  FMStateHandle AddEntry(TSubclassOf<UMStateDefinition> DefinitionClass);

  /**
   * @brief 既存のステートエントリーを削除
   * 
   * @param ステートハンドル
   */
  void RemoveEntry(const FMStateHandle& Handle);

  /**
   * @brief ステートの切り替えを試みる
   * 
   * @param CurrentStateInstance 現在のステート
   * @param NextStateTag 次のステートのタグ
   * 
   * @return 成功したら次のステートインスタンスのポインターを返す、それ以外はnullptr
   */
  UMStateInstance* TrySwitchState(const UMStateInstance* CurrentStateInstance, const FGameplayTag& NextStateTag);

  /**
   * @brief 特定のステートタグが存在しますかを確認する
   * 
   * @param StateTag 調べるタグ
   * 
   * @return 存在したらtrue、それ以外false
   */
  bool ContainsStateTag(const FGameplayTag& StateTag) const;

  /**
   * @brief タグでステートインスタンスを取得
   * 
   * @param StateTag タグ
   * 
   * @return 見つかったら有効なインスタンスポインターを返す、それ以外はnullptr
   */
  UMStateInstance* GetStateByTag(const FGameplayTag& StateTag) const; 
  
  /**
   * @brief ステートインスタンスでタグを取得する
   * 
   * @param StateInstance ステートインスタンスポインター
   * 
   * @return 有効なインスタンスポインターかつ実在すればステートのタグを返す、それ以外はFGameplayTag::EmptyTag
   */
  FGameplayTag GetTagByState(const UMStateInstance* StateInstance) const;

private:

  UPROPERTY()
  TArray<FMStateMachineStateListEntry> Entries;       // ステートエントリーコンテナ

  UPROPERTY()
  TObjectPtr<UMStateMachineComponent> OwnerComponent; // ステートを持つステートマシンコンポーネントポインター
};

/**
 * ステートマシン初期化パラメータ
 */
struct FStateMachineInitializationParameters
{
  TObjectPtr<UObject> Owner;

  TObjectPtr<AController> OwnerController;
};


/**
 * ステートマシンコンポーネント
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMStateMachineComponent : public UActorComponent
{
  GENERATED_BODY()

public:	
  MSTATEMACHINE_API UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:
  MSTATEMACHINE_API virtual void BeginPlay() override;

public:	

  //---UActorComponent Interface
  #pragma region UActorComponent Interface

  MSTATEMACHINE_API virtual void InitializeComponent() override;
  MSTATEMACHINE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  MSTATEMACHINE_API virtual void UninitializeComponent() override;

  #pragma endregion UActorComponent Interface
  //---End of UActorComponent Interface

  MSTATEMACHINE_API void Initialize(const FStateMachineInitializationParameters& Params);
  
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void StartTickState();

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void StopTickState();

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void SetEntryState(const FGameplayTag& EntryStateTag);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API TArray<FMStateHandle> AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API void RemoveState(FMStateHandle StateHandle);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  MSTATEMACHINE_API bool SwitchNextState(const FGameplayTag& NextStateTag);

  UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
  MSTATEMACHINE_API bool ContainsStateTag(const FGameplayTag& Tag) const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API FGameplayTag GetCurrentStateTag() const;

  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  MSTATEMACHINE_API FGameplayTag GetStateTagByInstance(const UMStateInstance* StateInstance) const;

  FMStateContext* GetContext() const { return m_context.Get(); }

  int32 GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const;


private:
  void EnterStateInternal(const UMStateInstance* PreviousStateInstance, UMStateInstance* NextStateInstance);
  void TickStateInternal(UMStateInstance* CurrentStateInstance, float DeltaTime);
  void ExitStateInternal(UMStateInstance* StateInstance, const UMStateInstance* NextStateInstance, const EStateExitReason Reason = EStateExitReason::Transition);

private:
  UPROPERTY()
  FMStateMachineStateList m_stateList;

  UPROPERTY()
  TObjectPtr<UMStateInstance> m_currentState;

  UPROPERTY(EditAnywhere)
  bool bAutoInitializeContext;

  TSharedPtr<FMStateContext, ESPMode::NotThreadSafe> m_context;

  uint8 m_bIsStateMachineStarted : 1;
  uint8 m_bCanTickStateMachine : 1;
    
};
