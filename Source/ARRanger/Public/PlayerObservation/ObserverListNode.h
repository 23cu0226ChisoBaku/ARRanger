#pragma once

#ifndef _AR_OBSERVER_NODE_
#define _AR_OBSERVER_NODE_

namespace ARRanger
{
  class FObserverProxyRootNode;

  class FObserverListNode : public TSharedFromThis<FObserverListNode>
  {
    friend class FObserverListRootNode;

    protected:
      FObserverListNode();

    public:
      ~FObserverListNode();

      void Initialize(FObserverListNode* NextListNodePtr = nullptr);
      void Evaluate();
      void BindNewObserver(const TDelegate<void()>& NewObservationEvent);
      void Reset();
      void SetNextListNode(const TSharedPtr<FObserverListNode> NextListNodePtr);
      FObserverListNode* GetNextListNode() const { return m_nextNodePtr.Get(); }

    private:
      TSharedPtr<FObserverListNode> m_nextNodePtr;
      
      TSharedPtr<FObserverProxyRootNode> m_proxyRootNodePtr;
  };
}

#endif // _AR_OBSERVER_NODE_