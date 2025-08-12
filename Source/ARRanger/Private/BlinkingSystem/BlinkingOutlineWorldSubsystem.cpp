//*************************************************
// アウトラインの点滅処理を制御するワールドシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineWorldSubsystem.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"
#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"

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
	m_BlinkingOutlineSystem.Reset();
	Super::Deinitialize();
}

void UBlinkingOutlineWorldSubsystem::SetupBlinkingSystem(UWorld* world, ULineTraceSingleARObjectComponent* lineTraceComp, TSubclassOf<ABlinkOutlineTickActor> tickActorClass)
{
	if (world == nullptr || lineTraceComp == nullptr || tickActorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlinkingOutlineWorldSubsystem: Setup failed due to null parameters."));
		return;
	}

	m_BlinkingOutlineSystem = MakeUnique<FBlinkingOutlineSystem>();
	m_BlinkingOutlineSystem->CreateTickingActor(world, tickActorClass);

	m_LineTraceComponent = lineTraceComp;
	m_TickActorClass = tickActorClass;

	BindBlinkingMagnetizableObjectDelegate();
}

/*
* @brief BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインド 
*/
void UBlinkingOutlineWorldSubsystem::BindBlinkingMagnetizableObjectDelegate()
{
	if (m_LineTraceComponent != nullptr && m_BlinkingOutlineSystem != nullptr)
	{
		m_LineTraceComponent->SetTargetMagnetizableObject.BindRaw(
			m_BlinkingOutlineSystem.Get(),
			&FBlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate
		);

		m_LineTraceComponent->UnsetTargetMagnetizableObject.BindRaw(
			m_BlinkingOutlineSystem.Get(),
			&FBlinkingOutlineSystem::UnsetTargetMagnetizableObjectDelegate
		);
	}
}

/*
* @brief バインドされているデリゲート関数をアンバインドする 
*/
void UBlinkingOutlineWorldSubsystem::UnBindDelegate()
{
	if (m_LineTraceComponent != nullptr)
	{
		m_LineTraceComponent->SetTargetMagnetizableObject.Unbind();
		m_LineTraceComponent->UnsetTargetMagnetizableObject.Unbind();
	}
}