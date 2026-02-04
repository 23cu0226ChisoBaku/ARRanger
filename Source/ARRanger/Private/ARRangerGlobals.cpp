// Fill out your copyright notice in the Description page of Project Settings.

#include "ARRangerGlobals.h"
#include "Framework/Application/IInputProcessor.h"
#include "GameplayFramework/ARGameInstance.h"

#include "Kismet/GameplayStatics.h"

class FARDebugInputProcessor : public IInputProcessor
{
  public:
    FARDebugInputProcessor(UARGameInstance* InGI)
      : m_gameInst(InGI)
    {
      check(InGI != nullptr);
    }

    void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
    { }

    bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
    {
      // Ignore Key that is combine with modifier key
      if (InKeyEvent.IsAltDown() || InKeyEvent.IsCommandDown() || InKeyEvent.IsControlDown() || InKeyEvent.IsShiftDown())
      {
        return false;
      }

      bool bKeyDownHandled = false;
      const FKey pressedKey = InKeyEvent.GetKey();
      // Reset Game button
      if (pressedKey == EKeys::F1)
      {
        if (m_gameInst.IsValid())
        {
          m_gameInst->ResetGame();
          bKeyDownHandled = true;
        }
      }
      else if (pressedKey == EKeys::F11)
      {
        if (m_gameInst.IsValid())
        {
          m_gameInst->ResetBattleTestStage();
          bKeyDownHandled = true;
        }
      }

      // For Demo Build
// #if AR_DEMO_BUILD

//       static const TMap<EKeys, FName> s_demoLevelMap =
//       {
//         {EKeys::F5, TEXT("Demo_Punch_Stage")},
//         {EKeys::F6, TEXT("Demo_ChargeKick_Special_Stage")},
//         {EKeys::F7, TEXT("Demo_AttractionClimb_Stage")},
//         {EKeys::F8, TEXT("Demo_RepulsionJump_Stage")}
//       };

//       if (s_demoLevelMap.Contains(pressedKey))
//       {
//         UGameplayStatics::OpenLevel(m_gameInst.Get(), s_demoLevelMap[pressedKey]);
//         bKeyDownHandled = true;
//       }
// #endif
      
      return bKeyDownHandled;
    }

    static TSharedPtr<FARDebugInputProcessor> MakeInstance(UARGameInstance* InGI)
    {
      return ::MakeShared<FARDebugInputProcessor>(InGI);
    }

  private:

    TWeakObjectPtr<UARGameInstance> m_gameInst = nullptr;
};

namespace ARRanger
{

namespace Global
{
  static TSharedPtr<IInputProcessor> s_debugInputProcessor{nullptr};

  void RegisterDebugKey()
  {
    if (GEngine != nullptr)
    {
      for (const FWorldContext& worldCtx : GEngine->GetWorldContexts())
      {
        UGameInstance* gameInst = worldCtx.OwningGameInstance;
        if (UARGameInstance* arGameInst = ::Cast<UARGameInstance>(gameInst))
        {
          if (s_debugInputProcessor.IsValid())
          {
            UnregisterDebugKey();
          }

          s_debugInputProcessor = FARDebugInputProcessor::MakeInstance(arGameInst);
          FSlateApplication::Get().RegisterInputPreProcessor(s_debugInputProcessor);
        }
      }
    }
  }

  void UnregisterDebugKey()
  {
    FSlateApplication::Get().UnregisterInputPreProcessor(s_debugInputProcessor);
    s_debugInputProcessor.Reset();
  }
} // namespace ARRanger::Global

} // namespace ARRanger
