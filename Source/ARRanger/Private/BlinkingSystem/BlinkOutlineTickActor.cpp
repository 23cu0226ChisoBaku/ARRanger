//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"


#define OUTLINEMATERIAL

/*
* Start ABlinkOutlineTickActor Lifecycle Functions
*/
ABlinkOutlineTickActor::ABlinkOutlineTickActor()
	: m_BlinkOutlineFunctor(new BlinkOutlineFunctor()) 
{
	PrimaryActorTick.bCanEverTick = true;

	// コールバック登録
    m_BlinkOutlineFunctor->OnBlinkEnd = [this](AActor* actor)
    {
        this->RemoveBlinkingActor(actor);
    };
}
void ABlinkOutlineTickActor::BeginPlay()
{
	Super::BeginPlay();
}
void ABlinkOutlineTickActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 登録されたアクターにアウトラインを付け点滅させる
	for (int32 i = 0; i < m_BlinkingActors.Num(); ++i)
	{
		if (m_BlinkingActors[i]._actor == nullptr || m_BlinkingActors[i]._meshComponent == nullptr ) {continue;}
		
		/*点滅させる(引数：対象アクター, 対象アクターのコンポーネント, 対象アクターに対応した点滅に関するデータ, １フレームの時間)*/ 
		m_BlinkOutlineFunctor->OutlineBlink(
			Cast<UObject>(m_BlinkingActors[i]._actor),
			m_BlinkingActors[i]._meshComponent,
			m_BlinkDatas.GetBlinkData(m_BlinkingActors[i]._actor),
			DeltaTime );
	}
}
ABlinkOutlineTickActor::~ABlinkOutlineTickActor()
{
	if(m_BlinkOutlineFunctor != nullptr)
	{
		delete m_BlinkOutlineFunctor;
		m_BlinkOutlineFunctor = nullptr;
	}
}
/*
* End ABlinkOutlineTickActor Lifecycle Functions
*/

/*
* @brief 点滅させるアクターを追加する
*
* @param 点滅させるアクター
*/
void ABlinkOutlineTickActor::AddBlinkingActor(AActor* newActor)
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
			break; // 最初に見つけたものでOKなら break
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
void ABlinkOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
{
	if (removeActor == nullptr){ return; }

	// 登録されているアクターかチェック
	if(!ContainsActor(removeActor)){ return; }

	// GetBlinkData()で該当のデータを取得
    FBlinkingActorData* blinkData = m_BlinkDatas.GetBlinkData(removeActor);
    if (blinkData)
    {
        // アウトライン解除
        UMeshComponent* meshComp = nullptr;
        for (const FBlinkingTarget& target : m_BlinkingActors)
        {
            if (target._actor == removeActor)
            {
                meshComp = target._meshComponent;
            }
        }
        if (meshComp && meshComp->GetOverlayMaterial())
        {
            meshComp->SetOverlayMaterial(nullptr);
        }

        // 動的マテリアルを解放・リセット
        blinkData->m_DynamicMaterial = nullptr;
        // 他の状態も必要に応じて初期化する
        blinkData->_elapsedTime = 0.f;

		// 登録されている配列から削除する
		m_BlinkingActors.RemoveAll([removeActor](const FBlinkingTarget& target) 
		{
			return target._actor == removeActor;
		});
	}
}

// /**
//  * @brief 指定されたアクターの EARMagnetismType を変更する
//  * 
//  * @param 変更するアクター, 変更先のEARMagnetismType
//  */
// void ABlinkOutlineTickActor::UpdateBlinkingDataByMagnetismType(AActor* actor, EARMagnetismType magnetismType)
// {
// 	if(actor == nullptr) {return;}
// 	if(!ContainsActor(actor)) {return;}

// 	// m_BlinkingActors に登録されているアクターのデータを取得

//     for (const FBlinkingTarget& target : m_BlinkingActors)
//     {
//         if (target._actor == actor)
//         {
//             break;
//         }
//     }
// }


/**
 * @brief m_BlinkingActors構造体配列の中に指定したアクターが存在するかどうかを返す
 * 
 * @param 探したいアクター
 * 
 * @return 存在したいかどうか
 */
bool ABlinkOutlineTickActor::ContainsActor(AActor* actor)
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