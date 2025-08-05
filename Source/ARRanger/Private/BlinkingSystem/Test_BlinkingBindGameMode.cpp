//*************************************************
// 引力斥力の付与に関する制御を行うゲームモード
//*************************************************

#include "Public/BlinkingSystem/Test_BlinkingBindGameMode.h"
#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* @brief アクターのBeginPlayの前に処理する
*/
void ATest_BlinkingBindGameMode::StartPlay()
{
     m_BlinkingOutlineSystem = new BlinkingOutlineSystem();

    // 妥協処理
    m_LineTraceComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<ULineTraceSingleARObjectComponent>();

    if (m_LineTraceComponent && m_BlinkingOutlineSystem)
    {
        m_LineTraceComponent->SetTargetMagnetizableObject.BindRaw(
            m_BlinkingOutlineSystem,
            &BlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate
        );
    }

    /* 引力斥力の付与に関するデリゲート関数をバインド*/
    BindSetTargetMagnetizableObject();
}

/*
* @brief BlinkingOutlineSystem にある引力斥力を付与するオブジェクトを保持するの関数を 
*        LineTraceSingleARObjectComponent のデリゲートにバインド 
*/
void ATest_BlinkingBindGameMode::BindSetTargetMagnetizableObject()
{
    if (m_LineTraceComponent && m_BlinkingOutlineSystem)
    {
        m_LineTraceComponent->SetTargetMagnetizableObject.BindRaw(
            m_BlinkingOutlineSystem,
            &BlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate
        );
    }
}