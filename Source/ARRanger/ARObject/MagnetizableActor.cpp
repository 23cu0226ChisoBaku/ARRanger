//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#include "ARObject/MagnetizableActor.h"
#include "Components/CapsuleComponent.h"

/*
* @brief コンストラクタ
*/
ARRANGER_API AMagnetizableActor::AMagnetizableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root（SceneComponent）を作成
	_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = _pRootComponent;

	// カプセルコンポーネントを作成
	_pMagneticField = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
}

/*
* @brief 毎フレーム処理
*/
ARRANGER_API void AMagnetizableActor::BeginPlay()
{
	Super::BeginPlay();
}

/*
* @brief ゲームプレイ開始後に一度だけ呼ばれる処理
*/
ARRANGER_API void AMagnetizableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* 引力状態に設定する関数
*/
ARRANGER_API void AMagnetizableActor::OnAttraction()
{
	// 共通処理;
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnAttraction Invoke")); }
}

/*
* 斥力状態に設定する関数
*/
ARRANGER_API void AMagnetizableActor::OnRepulsion()
{
	// 共通処理;
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnRepulsion Invoke")); }
}