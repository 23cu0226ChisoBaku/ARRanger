//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#include "ARObject/ARObjectBase.h"
#include "ARObjectComponent.h"

/*
* コンストラクタ
*/
AARObjectBase::AARObjectBase()
	: _pARObjectComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	// 引力斥力の挙動コンポーネントを生成。
	_pARObjectComponent = CreateDefaultSubobject<UARObjectComponent>(TEXT("Successfully created the ARObjectComponent."));
	if (!_pARObjectComponent){UE_LOG(LogTemp, Warning, TEXT("Failed to correctly create _pARObjectComponent!"));}
}

/*
* 引力状態に設定する関数
*/
void AARObjectBase::OnAttraction()
{
	if (_pARObjectComponent)
	{
		_pARObjectComponent->SetARType(EARType::Attraction);
	}
}

/*
* 斥力状態に設定する関数
*/
void AARObjectBase::OnRepulsion()
{
	if (_pARObjectComponent)
	{
		_pARObjectComponent->SetARType(EARType::Repulsion);
	}
}

/*
* 現在の状態を取得する関数(ARComponentにある変数を持ってくる)
*/
void AARObjectBase::SetNewARType(EARType newType) const
{
	if (_pARObjectComponent) {_pARObjectComponent->SetARType(newType);}
}

/*
* 現在の状態を取得する関数(ARComponentにある変数を持ってくる)
*/
EARType AARObjectBase::GetCurrentARType() const
{
	return _pARObjectComponent ? _pARObjectComponent->GetCurrentARType() : EARType::None;
}