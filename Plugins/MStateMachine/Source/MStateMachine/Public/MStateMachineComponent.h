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

#define UE_API MSTATEMACHINE_API

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

  UE_API FMStateHandle();
  UE_API FMStateHandle(const UMStateInstance* State, const UActorComponent* OwnerComp, const FGameplayTag& StateTag);

  /**
   * @brief Handle有効化チェック
   * 
   * @return インスタンス、コンポーネント、タグがすべて有効値だったらtrue、それ以外はfalseを返す
   */
  UE_API bool IsValid() const;

  /**
   * @brief ステートタグを取得
   * 
   * @return タグが無効だったらFGameplayTag::EmptyTagを返す
   */
  UE_API FGameplayTag GetStateTag() const;
  
private:

  /**ステートインスタンス弱参照 */
  TWeakObjectPtr<const UMStateInstance> m_state;
  
  /**Ownerコンポーネント弱参照 */
  TWeakObjectPtr<const UActorComponent> m_ownerComp;
  
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

  FMStateMachineStateListEntry();

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
  UE_API UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:

  /**Start UActorComponent interface */
  #pragma region UActorComponent Interface

  UE_API virtual void BeginPlay() override;  
public:	
  UE_API virtual void InitializeComponent() override;
  UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  UE_API virtual void UninitializeComponent() override;
  
  #pragma endregion UActorComponent Interface
  /**End UActorComponent interface */
  
  /**
   * @brief ステートマシンを初期化する
   * 
   * @param Params ステートマシン初期化専用パラメータ
   */
  UE_API void Initialize(const FStateMachineInitializationParameters& Params);
  
  /**
   * @brief ステートマシンを起動する
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API void StartTickState();

  /**
   * @brief ステートマシンを止める
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API void StopTickState();

  /**
   * @brief ステートマシンの初期ステートを設定
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API void SetEntryState(const FGameplayTag& EntryStateTag);

  /**
   * @brief 新しいステートを追加する
   * 
   * @param StateDefClass ステート定義オブジェクトのUClass
   * 
   * @return 追加成功したら有効なハンドル、それ以外は無効なハンドル
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

  /**
   * @brief 複数のステートを追加する
   * 
   * @param StateDefClasses ステート定義オブジェクトのUClass配列
   * @param OutHandles  　  ステートハンドル配列(ハンドルの順番はDefinitionの順番と同じ)
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API void AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses, TArray<FMStateHandle>& OutHandles);

  /**
   * @brief ステートを削除する
   * 
   * @param StateHandle ステートハンドル
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API void RemoveState(const FMStateHandle& StateHandle);

  /**
   * @brief 次のステートに切り替える
   * 
   * @param NextStateTag 次のステートのタグ
   * 
   * @return 切り替えが成功したらtrue, ステートが存在しない、タグが無効な場合はfalse 
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine")
  UE_API bool SwitchNextState(const FGameplayTag& NextStateTag);

  /**
   * @brief ステートタグが存在するかを確認する
   * 
   * @param Tag ステートタグ
   * 
   * @return 見つかったらtrue、それ以外はfalse
   */
  UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
  UE_API bool ContainsStateTag(const FGameplayTag& Tag) const;

  /**
   * @brief 次のステートに切り替えられるかを確認する
   * 
   * @param 次のステートタグ
   * 
   * @return 現在のステートが有効かつ次のステートに切り替えられるタグ情報に入力タグが入っていればtrue、
   *         それ以外はfalse
   */
  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  UE_API bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

  /**
   * @brief 今のステートタグを取得する
   * 
   * @return ステートが有効だったらステートにバインドされたタグを返す、それ以外はFGameplayTagを返す
   */
  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  UE_API FGameplayTag GetCurrentStateTag() const;

  /**
   * @brief ステートインスタンスでタグを取得する
   * 
   * @param StateInstance ステートインスタンスポインター
   * 
   * @return ステートインスタンスがステートマシンに入っていればステートタグを返す、
   *         それ以外はFGameplayTag::EmptyTagを返す
   */
  UFUNCTION(BlueprintPure, Category = "MStateMachine")
  UE_API FGameplayTag GetStateTagByInstance(const UMStateInstance* StateInstance) const;

  /**
   * @brief ステートコンテキストを取得する
   * 
   * @return ステートコンテキストポインター（初期化されていないとnullptrになる）
   */
  FMStateContext* GetContext() const { return m_context.Get(); }

  /**
   * @brief 今のステートから他のステートに遷移できるタグをすべて取得
   * 
   * @param OutTags 遷移できるステートタグが入っている配列
   * 
   * @return 見つけたステートタグの数
   */
  UE_API int32 GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const;

private:

  /**
   * @brief 新しいステートに入る
   * 
   * @param PreviousStateInstance 前のステート
   * @param NextStateInstance     次のステート
   */
  void EnterStateInternal(const UMStateInstance* PreviousStateInstance, UMStateInstance* NextStateInstance);

  /**
   * @brief 現在のステートを更新する
   *
   * @param CurrentStateInstance  現在のステート
   * @param DeltaTime             デルタタイム
   */
  void TickStateInternal(UMStateInstance* CurrentStateInstance, float DeltaTime);

  /**
   * @brief ステート終了処理
   * 
   * @param PreviousStateInstance 終了する予定のステート
   * @param NextStateInstance     次のステート
   * @param Reason                ステートを出る原因(列挙型)
   */
  void ExitStateInternal(UMStateInstance* PreviousStateInstance, const UMStateInstance* NextStateInstance, const EStateExitReason Reason = EStateExitReason::Transition);

private:
  UPROPERTY()
  FMStateMachineStateList m_stateList;        // ステートインスタンスなどの情報を持つリスト

  UPROPERTY()
  TObjectPtr<UMStateInstance> m_currentState; // 現在のステートインスタンス

  UPROPERTY(EditAnywhere, Category = "MStateMachine|Initialize", meta = (AllowPrivateAccess = "true"))
  bool bAutoInitializeContext;                // 自動的にコンテキストを初期化するかお表すフラグ

  TSharedPtr<FMStateContext, ESPMode::NotThreadSafe> m_context;   // コンテキスト
  uint8 m_bIsStateMachineStarted : 1;                             // ステートマシンが起動したかを表すフラグ
  uint8 m_bCanTickStateMachine : 1;                               // ステートマシンをTickできるかを表すフラグ
    
};

#undef UE_API
