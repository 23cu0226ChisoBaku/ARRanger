#include "PlayerObservation/ObserverListNode.h"
#include "PlayerObservation/ObserverProxyRootNode.h"


namespace ARRanger
{
  FObserverListNode::FObserverListNode()
    : m_nextNodePtr{nullptr}
    , m_proxyRootNodePtr{nullptr}
  { }

  FObserverListNode::~FObserverListNode()
  {
    Reset();
  }

  void FObserverListNode::Initialize(FObserverListNode* NextListNodePtr)
  {
    if (m_nextNodePtr != nullptr)
    {
      return;
    }

    if (NextListNodePtr != nullptr)
    {
      m_nextNodePtr = NextListNodePtr->AsShared();
    }

    m_proxyRootNodePtr = ::MakeShared<FObserverProxyRootNode>();
  }

  void FObserverListNode::Evaluate()
  {
    if (m_proxyRootNodePtr != nullptr)
    {
      m_proxyRootNodePtr->Evaluate();
    }

    if (m_nextNodePtr != nullptr)
    {
      m_nextNodePtr->Evaluate();
    }
  }

  void FObserverListNode::BindNewObserver(const TDelegate<void()>& NewObservationEvent)
  {
    if (m_proxyRootNodePtr == nullptr)
    {
      return;
    }

    m_proxyRootNodePtr->PushNodeBack(FObserverProxyRootNode::MakeNode(NewObservationEvent));
  }

  void FObserverListNode::Reset()
  {
    if (m_proxyRootNodePtr != nullptr)
    {
      m_proxyRootNodePtr->RemoveAll();
    }

    m_nextNodePtr.Reset();
  }

  void FObserverListNode::SetNextListNode(const TSharedPtr<FObserverListNode> NextListNodePtr)
  {
    if (this == NextListNodePtr.Get())
    {
      return;
    }

    m_nextNodePtr = NextListNodePtr;
  }
}