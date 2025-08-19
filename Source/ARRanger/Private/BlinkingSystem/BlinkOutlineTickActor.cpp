// //*************************************************
// // アウトラインの点滅処理を毎フレーム処理するアクター
// //*************************************************

// #include "Public/BlinkingSystem/BlinkOutlineTickActor.h"
// #include "BlinkingSystem/BlinkOutlineFunctor.h"

// /*
// * Start ABlinkOutlineTickActor Lifecycle Functions
// */
// ABlinkOutlineTickActor::ABlinkOutlineTickActor()
// 	: m_BlinkOutlineFunctor(new BlinkOutlineFunctor()) 
// {
// 	PrimaryActorTick.bCanEverTick = true;

// 	// コールバック登録
//     m_BlinkOutlineFunctor->OnBlinkEnd = [this](AActor* actor)
//     {
//         this->RemoveBlinkingActor(actor);
//     };
// }
// void ABlinkOutlineTickActor::BeginPlay()
// {
// 	Super::BeginPlay();
// }
// void ABlinkOutlineTickActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// 	// 参照配列に変更があれば反映
// 	if (isUpBlinkingActors)
// 	{
// 		UpdateBlinkingTargetsFromReference();
// 		isUpBlinkingActors = false;
// 	}

// 	// 登録されたアクターにアウトラインを付け点滅させる
// 	for (int32 i = 0; i < m_BlinkingActors.Num(); ++i)
// 	{
// 		if (m_BlinkingActors[i]._actor == nullptr || m_BlinkingActors[i]._meshComponent == nullptr ) {continue;}
		
// 		/*点滅させる(引数：対象アクター, 対象アクターのコンポーネント, 対象アクターに対応した点滅に関するデータ, １フレームの時間)*/ 
// 		m_BlinkOutlineFunctor->OutlineBlink(
// 			m_BlinkingActors[i]._actor,
// 			m_BlinkingActors[i]._meshComponent,
// 			m_BlinkDatas.GetBlinkData(m_BlinkingActors[i]._actor),
// 			DeltaTime );
// 	}

// 	// 登録されたアクターにアウトラインを付け点滅させる
// 	for (int32 i = 0; i < m_BlinkingActors.Num(); ++i)
// 	{
// 		if (m_BlinkingActors[i]._actor == nullptr || m_BlinkingActors[i]._meshComponent == nullptr ) {continue;}
		
// 		/*点滅させる(引数：対象アクター, 対象アクターのコンポーネント, 対象アクターに対応した点滅に関するデータ, １フレームの時間)*/ 
// 		m_BlinkOutlineFunctor->OutlineBlink(
// 			m_BlinkingActors[i]._actor,
// 			m_BlinkingActors[i]._meshComponent,
// 			m_BlinkDatas.GetBlinkData(m_BlinkingActors[i]._actor),
// 			DeltaTime );
// 	}
// }
// ABlinkOutlineTickActor::~ABlinkOutlineTickActor()
// {
// 	if(m_BlinkOutlineFunctor != nullptr)
// 	{
// 		delete m_BlinkOutlineFunctor;
// 		m_BlinkOutlineFunctor = nullptr;
// 	}
// }
// /*
// * End ABlinkOutlineTickActor Lifecycle Functions
// */

// /*
// * @brief FOutlineSystem のアウトライン配列を  で参照する
// * 
// * @param アウトラインを付ける対象アクター配列, 点滅するアウトラインを付ける対象アクター配列(OutLineSystemのオブジェクト配列ポインタ)
// */
// void ABlinkOutlineTickActor::BindOutlineActorArrays(TArray<TWeakObjectPtr<AActor>>* outlineActors, TArray<TWeakObjectPtr<AActor>>* blinkingActors)
// {
//     m_OutlineActorsReference = outlineActors;
//     m_BlinkingActorsReference = blinkingActors;
// 	isUpBlinkingActors = true;
// }

// /**
//  * @brief 点滅させる対象のアクターのデータ構造体から変更分を更新する
//  */
// void ABlinkOutlineTickActor::UpdateBlinkingTargetsFromReference()
// {
// 	if (!m_BlinkingActorsReference) return;

// 	// 変更分だけを処理
// 	TArray<AActor*> currentActors;
// 	for (FBlinkingTarget& target : m_BlinkingActors)
// 	{
// 		if (target._actor) currentActors.Add(target._actor);
// 	}

// 	// 追加
// 	for (TWeakObjectPtr<AActor> actorPtr : *m_BlinkingActorsReference)
// 	{
// 		AActor* actor = actorPtr.Get();
// 		if (actor == nullptr || currentActors.Contains(actor)) continue;

// 		// メッシュ取得
// 		UMeshComponent* meshComp = nullptr;
// 		TArray<UMeshComponent*> meshComponents;
// 		actor->GetComponents(meshComponents);
// 		for (UMeshComponent* comp : meshComponents)
// 		{
// 			if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
// 			{
// 				meshComp = comp;
// 				break;
// 			}
// 		}

// 		if (meshComp == nullptr) continue;

// 		FBlinkingTarget target;
// 		target._actor = actor;
// 		target._meshComponent = meshComp;
// 		m_BlinkingActors.Add(target);
// 	}

// 	// 削除
// 	m_BlinkingActors.RemoveAll([this](const FBlinkingTarget& target)
// 	{
// 		return !m_BlinkingActorsReference->Contains(target._actor);
// 	});
// }

// /*
// * @brief 点滅させるアクターを追加する
// *
// * @param 点滅させるアクター
// */
// void ABlinkOutlineTickActor::AddBlinkingActor(AActor* newActor)
// {
// 	if (newActor == nullptr){ return; }

// 	// すでに登録されているアクターかチェック
// 	if(ContainsActor(newActor)){return;}

// 	// メッシュコンポーネントを取得
// 	UMeshComponent* meshComponent = nullptr;
// 	TArray<UMeshComponent*> meshComponents;
// 	newActor->GetComponents(meshComponents);
// 	for (UMeshComponent* comp : meshComponents)
// 	{
// 		if (comp && comp->ComponentHasTag(TEXT("OutLineMeshComponent")))
// 		{
// 			meshComponent = comp;
// 			break;
// 		}
// 	}
// 	if (meshComponent == nullptr){ return; }

//     // 構造体を作成して追加
//     FBlinkingTarget newTarget;
//     newTarget._actor = newActor;
//     newTarget._meshComponent = meshComponent;

// 	// 点滅させるアクターとして登録
//     m_BlinkingActors.Add(newTarget);
// }

// /*
// * @brief 点滅をやめるアクターを配列から除外する
// *
// * @param 点滅をやめるアクター
// */
// void ABlinkOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
// {
// 	if (removeActor == nullptr){ return; }

// 	// 登録されているアクターかチェック
// 	if(!ContainsActor(removeActor)){ return; }

// 	// オブジェクトのタイプをNoneに変化
// 	if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(removeActor))
// 	{
// 		MagnetActor->SetMagnetismType(EARMagnetismType::None);
// 	}

// 	// GetBlinkData()で該当のデータを取得
//     FBlinkingActorData* blinkData = m_BlinkDatas.GetBlinkData(removeActor);
//     if (blinkData)
//     {
// 		// 妥協処理
// 		if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(removeActor))
// 		{
// 			MagnetActor->DynamicBlinkMaterial = nullptr;
// 			MagnetActor->ElapsedBlinkTime = 0.f;
// 			MagnetActor->SetCanSetMagnetismType(true);
// 		}	
		
// 		// // 動的マテリアルを解放・リセット
//         // blinkData->m_DynamicMaterial = nullptr;
//         // // タイマーリセット
//         // blinkData->_elapsedTime = 0.f;


//         // アウトライン解除
//         UMeshComponent* meshComp = nullptr;
//         for (const FBlinkingTarget& target : m_BlinkingActors)
//         {
//             if (target._actor == removeActor)
//             {
//                 meshComp = target._meshComponent;
// 				break;
//             }
//         }
//         if (meshComp != nullptr && meshComp->GetOverlayMaterial() != nullptr)
//         {
//             meshComp->SetOverlayMaterial(nullptr);
//         }

// 		// 登録されている配列から削除する
// 		m_BlinkingActors.RemoveAll([removeActor](const FBlinkingTarget& target) 
// 		{
// 			return target._actor == removeActor;
// 		});
// 	}
// }

// /**
//  * @brief カーソルのあった付与可能なオブジェクトを白く点滅させる
//  *
//  * @param 点滅させるアクター
//  */
// void ABlinkOutlineTickActor::BlinkOutlineActorAtCursor(AActor* targetObject)
// {
// 	if(targetObject != nullptr && m_BlinkOutlineFunctor != nullptr)
// 	{

// 	}
// }

// /**
//  * @brief m_BlinkingActors構造体配列の中に指定したアクターが存在するかどうかを返す
//  * 
//  * @param 探したいアクター
//  * 
//  * @return 存在したいかどうか
//  */
// bool ABlinkOutlineTickActor::ContainsActor(AActor* actor)
// {
//     if (actor == nullptr){ return false;}

//     for (const FBlinkingTarget& target : m_BlinkingActors)
//     {
//         if (target._actor == actor)
//         {
//             return true;
//         }
//     }
//     return false;
// }