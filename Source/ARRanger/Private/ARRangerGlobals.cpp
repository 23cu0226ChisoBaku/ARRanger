// Fill out your copyright notice in the Description page of Project Settings.


#include "ARRangerGlobals.h"
#include "Framework/Application/IInputProcessor.h"
#include "GameplayFramework/ARGameInstance.h"

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
      // Reset Game button
      const FKey pressedKey = InKeyEvent.GetKey();
      if (pressedKey == EKeys::F1)
      {
        if (m_gameInst.IsValid())
        {
          m_gameInst->ResetGame();
          return true;
        }
      }

      if (pressedKey == EKeys::F11)
      {
        if (m_gameInst.IsValid())
        {
          m_gameInst->ResetBattleTestStage();
          return true;
        }
      }
      

      return false;
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
