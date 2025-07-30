#include "PlayerObservation/Registry/SoundEffectRegistry.h"

#include "AudioSystem/ARSoundPlayLibrary.h"
#include "PlayerObservation/ObserverListNode.h"

namespace ARRanger
{
FSoundEffectRegistry::FSoundEffectRegistry(const FARSoundEffectParameters& Param, const TSharedPtr<IObserverRegistry>& Registry)
  : Super(Registry)
  , m_seParamPtr{nullptr}
{
  m_seParamPtr = ::MakeShared<FARSoundEffectParameters>(Param);
}

FSoundEffectRegistry::~FSoundEffectRegistry() = default;

void FSoundEffectRegistry::OnRegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers)
{
  check(m_seParamPtr.IsValid());
  if (Observers.IsValid())
  {
    FARSoundEffectParameters params = *m_seParamPtr;

    Observers->BindNewObserver(FSimpleDelegate::CreateLambda([params]()
    {
      UARSoundPlayLibrary::PlaySESound(params);
    }));
  }
}

} // namespace ARRanger
