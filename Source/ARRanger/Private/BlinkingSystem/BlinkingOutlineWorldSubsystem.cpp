//*************************************************
// アウトラインの点滅処理を制御するワールドシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineWorldSubsystem.h"
#include "Public/BlinkingSystem/DetectorMagnetizableComponent.h"
#include "Public/BlinkingSystem/OutlineTickActor.h"

/**
 * @brief 初期化処理
 */
void UBlinkingOutlineWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

/**
 * 
 */
void UBlinkingOutlineWorldSubsystem::Deinitialize()
{
	UnBindDelegate();
	m_OutlineSystem.Reset();
	Super::Deinitialize();
}

void UBlinkingOutlineWorldSubsystem::SetupBlinkingSystem(UWorld* world, UDetectorMagnetizableComponent* detectorMagnetizableComp, TSubclassOf<AOutlineTickActor> tickActorClass)
{
	if (world == nullptr || detectorMagnetizableComp == nullptr || tickActorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlinkingOutlineWorldSubsystem: Setup failed due to null parameters."));
		return;
	}

	m_OutlineSystem = MakeUnique<FOutlineSystem>();
	m_OutlineSystem->CreateTickingActor(world, tickActorClass);

	m_DetectorMagnetizableComponent = detectorMagnetizableComp;
	m_TickActorClass = tickActorClass;

	BindBlinkingMagnetizableObjectDelegate();
}

/*
* @brief OutlineSystem の関数を DetectorMagnetizableComponent のデリゲートにバインド 
*/
void UBlinkingOutlineWorldSubsystem::BindBlinkingMagnetizableObjectDelegate()
{
	if (m_DetectorMagnetizableComponent != nullptr && m_OutlineSystem != nullptr)
	{
		m_DetectorMagnetizableComponent->SetMagnetizableObjectAtCursor.BindRaw(
			m_OutlineSystem.Get(),
			&FOutlineSystem::BlinkOutlineActorAtCursorDelegate
		);
		m_DetectorMagnetizableComponent->UnsetMagnetizableObjectAtCursor.BindRaw(
			m_OutlineSystem.Get(),
			&FOutlineSystem::StopBlinkOutlineActorAtCursorDelegate
		);
		m_DetectorMagnetizableComponent->SetActorOnOutline.BindRaw(
			m_OutlineSystem.Get(),
			&FOutlineSystem::AddOutlineActorDelegate
		);
		m_DetectorMagnetizableComponent->UnsetActorOnOutline.BindRaw(
			m_OutlineSystem.Get(),
			&FOutlineSystem::RemoveOutlineActorDelegate
		);
		m_DetectorMagnetizableComponent->SetActorOnBlinkingOutline.BindRaw(
			m_OutlineSystem.Get(),
			&FOutlineSystem::AddBlinkingOutlineActorDelegate
		);

		
	}
}

/*
* @brief バインドされているデリゲート関数をアンバインドする 
*/
void UBlinkingOutlineWorldSubsystem::UnBindDelegate()
{
	if (m_DetectorMagnetizableComponent != nullptr)
	{
		m_DetectorMagnetizableComponent->SetMagnetizableObjectAtCursor.Unbind();
		m_DetectorMagnetizableComponent->UnsetMagnetizableObjectAtCursor.Unbind();
		m_DetectorMagnetizableComponent->SetActorOnOutline.Unbind();
		m_DetectorMagnetizableComponent->UnsetActorOnOutline.Unbind();
		m_DetectorMagnetizableComponent->SetActorOnBlinkingOutline.Unbind();
	}
}