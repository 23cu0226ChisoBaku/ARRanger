#include "PlayerObservation/ObserverProxy.h"

namespace ARRanger
{  
  FObserverProxy::FObserverProxy()
    : m_observationEvent{}
  { }

  void FObserverProxy::Evaluate()
  {
    (void)m_observationEvent.ExecuteIfBound();
  }

  void FObserverProxy::SetObservationEvent(const TDelegate<void()>& ObservationEvent)
  {
    m_observationEvent.Unbind();
    m_observationEvent = ObservationEvent;
  }

  bool FObserverProxy::IsValid() const
  {
    return m_observationEvent.IsBound();
  }
}