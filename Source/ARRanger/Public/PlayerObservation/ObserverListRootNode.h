#pragma once

#ifndef _AR_OBSERVER_LIST_ROOT_NODE_
#define _AR_OBSERVER_LIST_ROOT_NODE_

namespace ARRanger
{
  class FObserverListNode;

  class FObserverListRootNode
  {
    public:
      FObserverListRootNode();
      ~FObserverListRootNode();

      static TSharedPtr<FObserverListNode> MakeListNode();
      void AttachNewObserverList(const TSharedPtr<FObserverListNode>& NewListNode);
      void Reset();
      void Evaluate();

    private:
      TSharedPtr<FObserverListNode> m_listRootNode;
  };
}

#endif // _AR_OBSERVER_LIST_ROOT_NODE_
