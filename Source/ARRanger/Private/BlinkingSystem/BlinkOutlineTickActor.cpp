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
		UE_LOG(LogTemp, Log, TEXT("Tick: Actor=%s, Mesh=%s"),
		*GetNameSafe(m_BlinkingActors[i]._actor),
		*GetNameSafe(m_BlinkingActors[i]._meshComponent));

		if (m_BlinkingActors[i]._actor == nullptr || m_BlinkingActors[i]._meshComponent == nullptr ) {continue;}
		
		/*点滅させる(引数：対象アクター, 対象アクターのコンポーネント, 対象アクターに対応した点滅に関するデータ, １フレームの時間)*/ 
		m_BlinkOutlineFunctor->OutlineBlink(
			Cast<UObject>(m_BlinkingActors[i]._actor),
			m_BlinkingActors[i]._meshComponent,
			m_BlinkDatas.GetBlinkData(m_BlinkingActors[i]._actor),
			DeltaTime);
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
    UMeshComponent* meshComponent = newActor->FindComponentByClass<UMeshComponent>();
    if (!meshComponent){ return; }

    if (!meshComponent->ComponentHasTag(TEXT("OutLineMesh"))) { return; }

    // 構造体を作成して追加
    FBlinkingTarget newTarget;
    newTarget._actor = newActor;
    newTarget._meshComponent = meshComponent;

    m_BlinkingActors.Add(newTarget);

	UE_LOG(LogTemp, Log, TEXT("AddBlinkingActor: %s, Mesh: %s"),
    *GetNameSafe(newActor),
    *GetNameSafe(meshComponent));
}

/*
* @brief 点滅をやめるアクターを配列から除外する
*
* @param 点滅をやめるアクター
*/
void ABlinkOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
{
	if (removeActor == nullptr){ return; }

	// すでに登録されているアクターかチェック
	if(ContainsActor(removeActor)){return;}

    // メッシュコンポーネントを取得
    UMeshComponent* meshComponent = removeActor->FindComponentByClass<UMeshComponent>();
    if (!meshComponent){ return; }

    if (!meshComponent->ComponentHasTag(TEXT("OutLineMesh"))) { return; }

    // 構造体を作成して追加
    FBlinkingTarget removeTarget;
    removeTarget._actor = removeActor;
    removeTarget._meshComponent = meshComponent;

    m_BlinkingActors.Add(removeTarget);
}

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