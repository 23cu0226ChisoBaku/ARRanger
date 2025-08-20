//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#include "ARObject/MagnetizableActor.h"
#include "Components/CapsuleComponent.h"

/*
* @brief コンストラクタ
*/
AMagnetizableActor::AMagnetizableActor()
	: m_RootComponent (nullptr)
	, m_MagneticField (nullptr)
	, m_CanMagneticForce (true)
	, m_CanSetMagnetismType(true)
{
	PrimaryActorTick.bCanEverTick = true;

	// Root（SceneComponent）を作成
	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = m_RootComponent;

	// カプセルコンポーネントを作成
	m_MagneticField = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
}

/*
* @brief ゲームプレイ開始後に一度だけ呼ばれる処理
*/
void AMagnetizableActor::BeginPlay()
{
	Super::BeginPlay();
}

/*
* @brief 毎フレーム処理
*/
void AMagnetizableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* @brief 引力の磁性を保持している時の処理
*/
void AMagnetizableActor::OnAttractionEvaluated(const FARMagneticForceResult& Result)
{
	// 共通処理;
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnAttractionEvaluated Invoke")); }
}

/*
* @brief の磁性を保持している時の処理
*/
void AMagnetizableActor::OnRepulsionEvaluated(const FARMagneticForceResult& Result)
{
	// 共通処理;
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnRepulsionEvaluated Invoke")); }
}

/*テスト用*/
void AMagnetizableActor::SetType(EARMagnetismType NewType)
{
	if(m_CanSetMagnetismType)
	{
		SetMagnetismType(NewType);
	}
}