#include "PlayerObservation/ObserverProxyNode.h"

namespace ARRanger
{
  FObserverProxyNode::FObserverProxyNode(FObserverProxyNode::PrivateToken Token, const TSharedPtr<FObserverProxyNode>& NextNodePtr)
    : m_next(NextNodePtr)
    , m_proxy{}
  { }

  void FObserverProxyNode::InitializeNode(const TDelegate<void()>& ProxyEvent)
  {
    m_proxy.SetObservationEvent(ProxyEvent);
  }

  void FObserverProxyNode::SetNextNode(const TSharedPtr<FObserverProxyNode>& NextNodePtr)
  {
    if (m_next == NextNodePtr)
    {
      return;
    }

    m_next = NextNodePtr;
  }


  void FObserverProxyNode::Evaluate()
  {
    m_proxy.Evaluate();
    if (m_next.IsValid())
    {
      m_next->Evaluate();
    }
  }
      
}