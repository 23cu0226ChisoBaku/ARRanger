#pragma once

#ifndef _AR_PLAYER_NOTIFY_HANDLER_
#define _AR_PLAYER_NOTIFY_HANDLER_

#include "PlayerObservation/INotifyHandlerInterface.h"

#include "PlayerObservation/ObserverListRootNode.h"

namespace ARRanger::Player
{
  class FPlayerNotifyHandler : public ARRanger::INotifyHandlerInterface
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
      ARRanger::FObserverListRootNode DashObserverRoot;
      ARRanger::FObserverListRootNode JumpObserverRoot;
      ARRanger::FObserverListRootNode AttackObserverRoot;
  
  };
}

#endif // _AR_PLAYER_NOTIFY_HANDLER_