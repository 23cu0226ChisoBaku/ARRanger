#include "Test/TestNotifyHandler.h"

namespace ARRanger::Test
{
  void FTestNotifyHandler::OnDash()
  {

  }

  void FTestNotifyHandler::OnJump()
  {
    JumpObserverRoot.Evaluate();
  }

  void FTestNotifyHandler::OnAttack()
  {
    
  }

}