#pragma once

#ifndef _TEST_NOTIFY_HANDLER_
#define _TEST_NOTIFY_HANDLER_

#include "PlayerObservation/INotifyHandlerInterface.h"
#include "PlayerObservation/ObserverListRootNode.h"

namespace ARRanger::Test
{
  struct FTestNotifyHandler : public ARRanger::INotifyHandlerInterface
  {
    public:
      FTestNotifyHandler() = default;
      ~FTestNotifyHandler() = default;

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

#endif // _TEST_NOTIFY_HANDLER_