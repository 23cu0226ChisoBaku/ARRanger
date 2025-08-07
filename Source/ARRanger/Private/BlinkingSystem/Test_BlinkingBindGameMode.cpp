//*************************************************
// 引力斥力の付与に関する制御を行うゲームモード
//*************************************************

#include "Public/BlinkingSystem/Test_BlinkingBindGameMode.h"
#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

ATest_BlinkingBindGameMode::ATest_BlinkingBindGameMode()
    : m_LineTraceComponent{nullptr}
    , m_BlinkingOutlineSystem{nullptr}
{ }

/*
* @brief アクターのBeginPlayの前に処理する
*/
void ATest_BlinkingBindGameMode::StartPlay()
{
    m_BlinkingOutlineSystem = ::MakeUnique<FBlinkingOutlineSystem>();

    // 妥協処理
    m_LineTraceComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<ULineTraceSingleARObjectComponent>();

    /* 引力斥力の付与に関するデリゲート関数をバインド*/
    BindBlinkingMagnetizableObjectDelegate();
}

/*
* @brief BlinkingOutlineSystem にある引力斥力に関する関数を
*        LineTraceSingleARObjectComponent のデリゲートにバインド 
*/
void ATest_BlinkingBindGameMode::BindBlinkingMagnetizableObjectDelegate()
{
    if (m_LineTraceComponent && m_BlinkingOutlineSystem)
    {
        // 対象を点滅処理のオブジェクトに設定するデリゲート関数をバインド
        m_LineTraceComponent->SetTargetMagnetizableObject.BindRaw(
            m_BlinkingOutlineSystem.Get(),
            &FBlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate
        );

        // 対象を点滅処理のオブジェクトから除外するデリゲート関数をバインド
        m_LineTraceComponent->UnsetTargetMagnetizableObject.BindRaw(
            m_BlinkingOutlineSystem.Get(),
            &FBlinkingOutlineSystem::UnsetTargetMagnetizableObjectDelegate
        );
    } 
}

/*
* @brief バインドされているデリゲート関数をアンバインドする 
*/
void ATest_BlinkingBindGameMode::UnBindDelegate()
{
    if (m_LineTraceComponent)
    {
        m_LineTraceComponent->SetTargetMagnetizableObject.Unbind();
        m_LineTraceComponent->UnsetTargetMagnetizableObject.Unbind();
    }
}

/**
 * @brief ゲーム終了のタイミングで呼び出される
 */
void ATest_BlinkingBindGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnBindDelegate();
}