#pragma once

#ifndef _AR_OBSERVER_PROXY_
#define _AR_OBSERVER_PROXY_

namespace ARRanger
{
  struct FObserverProxy
  {
    FObserverProxy();
    ~FObserverProxy() = default;
    FObserverProxy(const FObserverProxy& Other) = default;
    FObserverProxy& operator=(const FObserverProxy& Other) = default;
    FObserverProxy(FObserverProxy&& Other) = default;
    FObserverProxy& operator=(FObserverProxy&& Other) = default;

    /**
     * @brief オブサーバー代理イベントを実行する
     */
    void Evaluate();
    void SetObservationEvent(const TDelegate<void()>& ObservationEvent);
    bool IsValid() const;

    private:
      TDelegate<void()> m_observationEvent;
  };
}

#endif // _AR_OBSERVER_PROXY_