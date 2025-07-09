// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UActorComponent を使用するためのインクルード
// UE のコンポーネントべース設計の基本クラス
// アクターに機能を追加する「コンポーネント」を定義・使用するため
#include "Components/ActorComponent.h"

// GamePlay Tags を使うためのインクルード
// タグを使って状態やカテゴリを柔軟に管理できる
// 状態や属性を表す「ゲームプレイタグ」を使用するため
#include "GameplayTagContainer.h"

// LogMState と LogMStateMachine というログカテゴリ実体（定義）を行っている
#include "MStateMachineLogChannels.h"

// 特殊なプリプロセッサディレクティブ
// Unreal Engine の リフレクション機能（UCLASS/USTRUCT/UENUM など）を正しく使うため
// UCLASS(), USTRUCT(), UENUM() などのマクロを使っている場合、この .generated.h を必ず クラス宣言の直前にインクルードする必要がある
#include "MStateMachineComponent.generated.h"

class UMStateInstance;
class UMStateDefinition;

// FMStateHandle という 構造体をブループリントから使える（変数として持てるようにする）
USTRUCT(BlueprintType)
struct FMStateHandle
{
	GENERATED_BODY()
	
	friend class UMStateMachineComponent;

	// FMStateHandle 構造体の デフォルトコンストラクタ の定義
	// 初期値を明示的に設定するコンストラクタ
	
	// コンストラクタ
	// nullptr にしておくことで、未初期化のポインタ参照によるクラッシュ（野良ポインタ）を防ぐ
	// FGameplayTag::EmptyTag を使うことで「このハンドルにはまだタグが設定されていない」ことを明示できる。
	// 初期化を明確にすることで、後の IsValid() などのチェックがしやすくなる。
	FMStateHandle()
		: m_state(nullptr)						
		, m_ownerComp(nullptr)
		, m_stateTag(FGameplayTag::EmptyTag)
	{
	}

	// コンストラクタ
	// 指定された値でハンドル構造体を初期化
	FMStateHandle(
		UMStateInstance* state,
		UActorComponent* ownerComp,
		const FGameplayTag& stateTag
							 )
		: m_state(state)
		, m_ownerComp(ownerComp)
		, m_stateTag(stateTag)
	{
	}

	// 有効であるか確認する関数
	bool IsValid() const;
	// ステートのタグを取得する関数
	// FGameplayTagはUEで標準用意されている構造体
	FGameplayTag GetStateTag() const;
	
private:
	// TWeakObjectPtr とは
	// 消滅の可能性がある UObject を安全に参照したいときに使うスマートポインタ
	// GC で削除されたかをチェックできる・オブジェクトがまだ生きていればアクセスできる
	TWeakObjectPtr<UMStateInstance> m_state;		// ステート保管用
	TWeakObjectPtr<UActorComponent> m_ownerComp;	// オーナーコンポネント保管用
	FGameplayTag m_stateTag;						// ステートのタグ
};


// この構造体を Unreal のリフレクションシステムに登録する
// （シリアライズ・エディタで参照できる）
USTRUCT()
struct FMStateMachineStateListEntry
{
	GENERATED_BODY()

	// ステートマシンが持つ状態一覧
	friend struct FMStateMachineStateList;
	friend class UMStateMachineComponent;

	FMStateMachineStateListEntry()
		: State(nullptr)
		, StateDefinition(nullptr)	// ステートの定義
	{
	}

private:
	// TObject = GC 対象
	// TObjectPtr：CGに対して安全な宣言
	UPROPERTY()
	TObjectPtr<UMStateInstance> State;

	UPROPERTY()
	TObjectPtr<const UMStateDefinition> StateDefinition;
};

// ステートの状態を管理する
USTRUCT()
struct FMStateMachineStateList
{
	GENERATED_BODY()

	// ステート
public:
	// コンストラクタ
	FMStateMachineStateList()
		: Entries{}
		, OwnerComponent(nullptr)
	{
	}

	// コンストラクタ
	FMStateMachineStateList(UActorComponent* ownerComp)
		: Entries{}
		, OwnerComponent(ownerComp)
	{
	}

	// 関数宣言	
	FMStateHandle AddEntry(TSubclassOf<UMStateDefinition>);
	void RemoveEntry(FMStateHandle);

	// 関数宣言
	UMStateInstance* SwitchState(const UMStateInstance* currentStateInstance, FGameplayTag);
	bool ContainsStateTag(const FGameplayTag&) const;
	UMStateInstance* GetStateByTag(const FGameplayTag&) const; 
	FGameplayTag GetTagByState(const UMStateInstance*) const;

private:

	friend class UMStateMachineComponent;

	UPROPERTY()
	TArray<FMStateMachineStateListEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};






// 状態遷移ステートマシンを管理する ActorComponent 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MSTATEMACHINE_API UMStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMStateMachineComponent(const FObjectInitializer& = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:	

	// 折りたためるように regionしてる
	//---UActorComponent Interface
	#pragma region UActorComponent Interface
	// コンポーネントが初期化されるときに呼ばれる関数(BeginPlay() よりも早く呼ばれる)
	virtual void InitializeComponent() override;
	// 毎フレーム処理
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// コンポーネントの終了時に呼ばれる
	virtual void UninitializeComponent() override;
	#pragma endregion UActorComponent Interface
	//---End of UActorComponent Interface

	// ステートの Tick（毎フレーム更新）を開始
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StartTickState();

	// ステートの Tick を停止
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void StopTickState();

	// 開始状態を指定する
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void SetEntryState(const FGameplayTag& EntryStateTag);

	// 新しいステートを追加して FMStateHandle を返す
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	FMStateHandle AddNewState(TSubclassOf<UMStateDefinition> StateDefClass);

	// 複数ステートをまとめて追加
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	TArray<FMStateHandle> AddStates(const TArray<TSubclassOf<UMStateDefinition>>& StateDefClasses);

	// 特定のステートを削除
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	void RemoveState(FMStateHandle StateHandle);

	// 指定されたタグのステートに遷移
	UFUNCTION(BlueprintCallable, Category = "MStateMachine")
	bool SwitchNextState(const FGameplayTag& NextStateTag);

	// 指定されたタグのステートがあるかを判定
	UFUNCTION(BlueprintCallable, Category = "MStateMachine|Data")
	bool ContainsStateTag(const FGameplayTag& Tag) const;

	// 現在の状態からそのタグに遷移できるか
	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	bool CanSwitchToNext(const FGameplayTag& NextStateTag) const;

	// 現在のステートタグを取得
	UFUNCTION(BlueprintPure, Category = "MStateMachine")
	FGameplayTag GetCurrentStateTag() const;

private:
	UPROPERTY()
	FMStateMachineStateList m_stateList;	// ステート一覧

	UPROPERTY()
	TObjectPtr<UMStateInstance> m_currentState;	// 現在有効な UMStateInstance

	uint8 m_bIsStateMachineStarted : 1;	// ステートマシンが開始されているか（1bit フラグ）
	uint8 m_bCanTickStateMachine : 1;	// Tick処理が有効かどうか
};
