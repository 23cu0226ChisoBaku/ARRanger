#include "PlayerObservation/PlayerNotifyHandler.h"

namespace ARRanger::Player
{
  void FPlayerNotifyHandler::OnDash()
  {
    DashObserverRoot.Evaluate();
  }

  void FPlayerNotifyHandler::OnJump()
  {
    JumpObserverRoot.Evaluate();
  }

  void FPlayerNotifyHandler::OnAttack()
  {
    AttackObserverRoot.Evaluate();
  }
}