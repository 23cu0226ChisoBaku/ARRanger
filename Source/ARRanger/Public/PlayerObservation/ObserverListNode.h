#pragma once

#ifndef _AR_OBSERVER_NODE_
#define _AR_OBSERVER_NODE_

#error Start Here
#include ""

namespace ARRanger
{
  struct FObserverListNode
  {
    public:
      FObserverListNode();
      ~FObserverListNode() = default;

    private:
      TSharedPtr<FObserverListNode> m_nextNode;
      TDelegate<void()> m_observer;
  };
}

#endif // _AR_OBSERVER_NODE_