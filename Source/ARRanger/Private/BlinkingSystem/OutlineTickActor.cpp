//*************************************************
// アウトラインの処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/OutlineTickActor.h"
#include "BlinkingSystem/BlinkOutlineFunctor.h"

/*
* Start AOutlineTickActor Lifecycle Functions
*/
AOutlineTickActor::AOutlineTickActor()
	: m_BlinkOutlineFunctor(new BlinkOutlineFunctor()) 
{
	PrimaryActorTick.bCanEverTick = true;

	// コールバック登録
    m_BlinkOutlineFunctor->OnBlinkEnd = [this](AActor* actor)
    {
        this->RemoveBlinkingActor(actor);
    };
}
void AOutlineTickActor::BeginPlay()
{
	Super::BeginPlay();
}
void AOutlineTickActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 参照配列に変更があれば反映
	UpdateOutlineTargets();
	//UpdateBlinkingTargets();

	if(m_BlinkingActorAtAtCursor != nullptr)
	{
		BlinkOutlineActorAtCursor(DeltaTime);
	}

	// 登録されたアクターにアウトラインを付け点滅させる
	for (int32 i = 0; i < m_BlinkingActors.Num(); ++i)
	{
		if (m_BlinkingActors[i]._actor == nullptr || m_BlinkingActors[i]._meshComponent == nullptr ) {continue;}
		
		/*点滅させる(引数：対象アクター, 対象アクターのコンポーネント, 対象アクターに対応した点滅に関するデータ, １フレームの時間)*/ 
		m_BlinkOutlineFunctor->OutlineBlink(
			m_BlinkingActors[i]._actor,
			m_BlinkingActors[i]._meshComponent,
			m_BlinkDatas.GetBlinkData(m_BlinkingActors[i]._actor),
			DeltaTime );
	}

	/*デバッグ用*/
	// if (GEngine)
	// {
	// 	for (int32 i = 0; i < m_BlinkingActors.Num(); ++i)
	// 	{
	// 		const FBlinkingTarget& Target = m_BlinkingActors[i];
	// 		FString ActorName = Target._actor ? Target._actor->GetName() : TEXT("None");

	// 		FString LogMessage = FString::Printf(TEXT("BlinkingActor[%d]: %s"), i, *ActorName);

	// 		GEngine->AddOnScreenDebugMessage(
	// 			-1,                // Key (-1 = 自動割り当て)
	// 			5.0f,              // 表示時間（秒）
	// 			FColor::Green,     // 色
	// 			LogMessage         // 表示内容
	// 		);
	// 	}
	// }

	// if (GEngine && m_BlinkingActorsReference)
	// {
	// 	for (int32 i = 0; i < m_BlinkingActorsReference->Num(); ++i)
	// 	{
	// 		AActor* Target = (*m_BlinkingActorsReference)[i].Get();
	// 		FString ActorName = Target ? Target->GetName() : TEXT("None");

	// 		FString LogMessage = FString::Printf(TEXT("BlinkingActor[%d]: %s"), i, *ActorName);

	// 		GEngine->AddOnScreenDebugMessage(
	// 			-1,                // Key (-1 = 自動割り当て)
	// 			5.0f,              // 表示時間（秒）
	// 			FColor::Blue,      // 色
	// 			LogMessage         // 表示内容
	// 		);
	// 	}
	// }
}
AOutlineTickActor::~AOutlineTickActor()
{
	if(m_BlinkOutlineFunctor != nullptr)
	{
		delete m_BlinkOutlineFunctor;
		m_BlinkOutlineFunctor = nullptr;
	}
}
/*
* End AOutlineTickActor Lifecycle Functions
*/

/*
* @brief FOutlineSystem のアウトライン配列を参照する
* 
* @param アウトラインを付ける対象アクター配列, 点滅するアウトラインを付ける対象アクター配列(OutLineSystemのオブジェクト配列ポインタ)
*/
void AOutlineTickActor::BindOutlineActorArrays(TArray<TWeakObjectPtr<AActor>>* outlineActors, TArray<TWeakObjectPtr<AActor>>* blinkingActors)
{
    // アウトライン
	m_OutlineActorsReference = outlineActors;
	// ブリンキング	
	m_BlinkingActorsReference = blinkingActors;	
}

/**
 * @brief 対象のオブジェクトにアウトラインを適用する
 */
void AOutlineTickActor::UpdateOutlineTargets()
{
	if (m_OutlineActorsReference == nullptr) return;

	// 新しい参照リスト（仮定：TArray<AActor*> など）
    TArray<TWeakObjectPtr<AActor>>& currentActors = *m_OutlineActorsReference;

	// すでに管理しているアクターと比較して差分を見つける
	for (TWeakObjectPtr<AActor> actorPtr : *m_OutlineActorsReference)
	{
		AActor* targetactor = actorPtr.Get();
		if (targetactor == nullptr) continue;
	
		// タグ付きのメッシュコンポーネントを探す
		UMeshComponent* meshComp = nullptr;
		TArray<UMeshComponent*> meshComponents;
		targetactor->GetComponents(meshComponents);
		for (UMeshComponent* comp : meshComponents)
		{
			if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
			{
				meshComp = comp;
				break;
			}
		}

		if(meshComp == nullptr)
		{
			return;
		}
		// リストに合ってアウトラインが適用されていないオブジェクトにアウトラインを適用
		if(meshComp->GetOverlayMaterial() == nullptr)
		{
			meshComp->SetOverlayMaterial(GetOutlineMaterial(targetactor));;
		}

		// すでに追加済みかチェック
		bool bAlreadyRegistered = false;
		for (const FBlinkingTarget& target : m_OutlineActors)
		{
			if (target._actor == targetactor)
			{
				bAlreadyRegistered = true;
				break;
			}
		}
		if (bAlreadyRegistered) {continue;}


		if (meshComp == nullptr) {continue;}

		// アウトライン適用
		meshComp->SetOverlayMaterial(GetOutlineMaterial(targetactor));

		// m_OutlineActors に追加
		FBlinkingTarget newTarget;
		newTarget._actor = targetactor;
		newTarget._meshComponent = meshComp;
		m_OutlineActors.Add(newTarget);
	}

	// 削除するターゲットを一時保持
	TArray<int32> RemovetargetsIndex;
	// リストからなくなったものは削除
	for (int32 i = m_OutlineActors.Num() - 1; i >= 0; --i)
    {
        FBlinkingTarget& target = m_OutlineActors[i];
        if (!target._actor || !currentActors.Contains(target._actor))
        {
            // アウトライン解除
            if (target._meshComponent)
            {
                target._meshComponent->SetOverlayMaterial(nullptr);
            }
			// 削除するリストに追加する
			RemovetargetsIndex.Add(i);
        }
    }

	for(int32 idx : RemovetargetsIndex)
	{
		// リストから削除
        m_OutlineActors.RemoveAt(idx);
	}
}


/**
 * @brief 点滅させる対象のアクターのデータ構造体から変更分を更新する
 */
void AOutlineTickActor::UpdateBlinkingTargets()
{
	if (m_BlinkingActorsReference == nullptr) {return;}

	// 変更分だけを処理
	TArray<AActor*> currentActors;
	for (FBlinkingTarget& target : m_BlinkingActors)
	{
		if (target._actor != nullptr) 
		{
			currentActors.Add(target._actor);
		}
	}

	// 新しい対象を追加
	for (TWeakObjectPtr<AActor> actorPtr : *m_BlinkingActorsReference)
	{
		AActor* actor = actorPtr.Get();
		if (actor == nullptr || currentActors.Contains(actor)) {continue;}

		// メッシュ取得
		UMeshComponent* meshComp = nullptr;
		TArray<UMeshComponent*> meshComponents;
		actor->GetComponents(meshComponents);
		for (UMeshComponent* comp : meshComponents)
		{
			if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
			{
				meshComp = comp;
				break;
			}
		}

		if (meshComp == nullptr) continue;

		FBlinkingTarget target;
		target._actor = actor;
		target._meshComponent = meshComp;
		m_BlinkingActors.Add(target);
	}
}

/*
* @brief 点滅させるアクターを追加する
*
* @param 点滅させるアクター
*/
void AOutlineTickActor::AddBlinkingActor(AActor* newActor)
{
	if (newActor == nullptr){ return; }

	// すでに登録されているアクターかチェック
	if(ContainsActor(newActor)){return;}

	// メッシュコンポーネントを取得
	UMeshComponent* meshComponent = nullptr;
	TArray<UMeshComponent*> meshComponents;
	newActor->GetComponents(meshComponents);
	for (UMeshComponent* comp : meshComponents)
	{
		if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
		{
			meshComponent = comp;
			break;
		}
	}
	if (meshComponent == nullptr){ return; }

    // 構造体を作成して追加
    FBlinkingTarget newTarget;
    newTarget._actor = newActor;
    newTarget._meshComponent = meshComponent;

	// 点滅させるアクターとして登録
    m_BlinkingActors.Add(newTarget);
}

/*
* @brief 点滅をやめるアクターを配列から除外する
*
* @param 点滅をやめるアクター
*/
void AOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
{
	if (removeActor == nullptr){ return; }

	// 登録されているアクターかチェック
	if(!ContainsActor(removeActor)){ return; }

	// オブジェクトのタイプをNoneに変化
	if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(removeActor))
	{
		MagnetActor->SetMagnetismType(EARMagnetismType::None);
	}

	// 妥協処理
	if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(removeActor))
	{
		MagnetActor->DynamicBlinkMaterial = nullptr;
		MagnetActor->ElapsedBlinkTime = 0.f;
		MagnetActor->SetCanSetMagnetismType(true);
	}	
	
	// アウトライン解除
	UMeshComponent* meshComp = nullptr;
	for (const FBlinkingTarget& target : m_BlinkingActors)
	{
		if (target._actor == removeActor)
		{
			meshComp = target._meshComponent;
			break;
		}
	}
	if (meshComp != nullptr && meshComp->GetOverlayMaterial() != nullptr)
	{
		meshComp->SetOverlayMaterial(nullptr);
	}

	// 登録されている配列から削除する
	m_BlinkingActors.RemoveAll([removeActor](const FBlinkingTarget& target) 
	{
		return target._actor == removeActor;
	});

	// システム側の配列からも削除
	if (OnRequestRemoveBlinkingActor != nullptr)
	{
		OnRequestRemoveBlinkingActor(removeActor);
	}
}

/**
 * @brief カーソルのあった付与可能なオブジェクトを設定する
 *
 * @param 点滅させるアクター
 */
void AOutlineTickActor::SetBlinkOutlineActorAtCursor(AActor* targetObject)
{
	if(targetObject != nullptr)
	{
		m_BlinkingActorAtAtCursor = targetObject;
	}
}

/**
 * @brief カーソルが外れた付与可能なオブジェクトを解除する
 * 
 * @param 解除するアクター
 */
void AOutlineTickActor::UnsetBlinkOutlineActorAtCursor(AActor* targetObject)
{
	m_BlinkingActorAtAtCursor = nullptr;
	if(targetObject != nullptr)
	{
		m_BlinkOutlineFunctor->ResetMaterialParam(targetObject);
	}
}

/**
 * @brief カーソルのある付与可能なオブジェクトのアウトラインを点滅させる
 *
 * @param １フレームの経過時間
 */
void AOutlineTickActor::BlinkOutlineActorAtCursor(float deltaTime)
{
	if (m_BlinkingActorAtAtCursor == nullptr) { return; }
	AMagnetizableActor* magnetActor = Cast<AMagnetizableActor>(m_BlinkingActorAtAtCursor);
	if(magnetActor == nullptr) {return;}
	if(magnetActor->GetMagnetismType() != EARMagnetismType::None)
	{
		return;
	}
	
	// メッシュコンポーネントを取得
	UMeshComponent* meshComponent = nullptr;
	TArray<UMeshComponent*> meshComponents;
	m_BlinkingActorAtAtCursor->GetComponents(meshComponents);
	for (UMeshComponent* comp : meshComponents)
	{
		if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
		{
			meshComponent = comp;
			break;
		}
	}
	if(meshComponent == nullptr){return;}

	AActor* targetActor = m_BlinkingActorAtAtCursor.Get();
	if (!targetActor) { return; }

	//点滅処理
	m_BlinkOutlineFunctor->OutlineBlink
	(	targetActor, 
		meshComponent, 
		m_BlinkDatas.GetBlinkData(targetActor), 
		deltaTime);
}

/**
 * @brief m_BlinkingActors構造体配列の中に指定したアクターが存在するかどうかを返す
 * 
 * @param 探したいアクター
 * 
 * @return 存在したいかどうか
 */
bool AOutlineTickActor::ContainsActor(AActor* actor)
{
    if (actor == nullptr){ return false;}

    for (const FBlinkingTarget& target : m_BlinkingActors)
    {
        if (target._actor == actor)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief そのアクターの状態にあったマテリアルを返す関数
 * 
 * @param アウトラインを付ける対象アクター 
 * 
 * @return アウトラインのマテリアル
 */
UMaterialInterface* AOutlineTickActor::GetOutlineMaterial(AActor* targetActor)
{
	if(targetActor == nullptr)
	{
		return m_BlinkDatas.GetNoneBlinkMaterial();
	}

	return m_BlinkDatas.GetBlinkData(targetActor)->_blinkMaterial;
}