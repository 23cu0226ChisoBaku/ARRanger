#pragma once

#ifndef _AR_PLAYER_NOTIFY_HANDLER_
#define _AR_PLAYER_NOTIFY_HANDLER_

#include "INotifyHandlerInterface.h"

namespace ARRanger::Player
{
  class FPlayerNotifyHandler final : public ARRanger::INotifyHandlerInterface
  {
    public:
      FPlayerNotifyHandler() = default;
      ~FPlayerNotifyHandler() = default;

    /**Start INotifyHandlerInterface interface */

    void OnDash() override final;

    void OnJump() override final;

    void OnAttack() override final;

    /**End INotifyHandlerInterface interface */

    public:
  
  };
}

#endif // _AR_PLAYER_NOTIFY_HANDLER_