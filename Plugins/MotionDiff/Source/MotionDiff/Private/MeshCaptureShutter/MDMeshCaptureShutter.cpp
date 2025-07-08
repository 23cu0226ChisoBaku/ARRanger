#include "MeshCaptureShutter/MDMeshCaptureShutter.h"

#include "MeshCapture/MDMeshCapture.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

namespace MotionDiff
{
  FMDMeshCaptureShutter_IntervalTime::FMDMeshCaptureShutter_IntervalTime(UMDMeshCapture* MeshCapture, const float TriggerInterval)
    : m_meshCapturePtr{MeshCapture}
    , m_triggerIntervalTime(TriggerInterval)
    , m_triggeredTimerHandle{}
    , m_nameIndex{0}
    , m_repeatCount{EShutterTriggerMode::Infinity}
  {

  }

  FMDMeshCaptureShutter_IntervalTime::FMDMeshCaptureShutter_IntervalTime(const FMDMeshCaptureShutter_IntervalTime& Other) noexcept
    : m_meshCapturePtr{Other.m_meshCapturePtr}
    , m_triggerIntervalTime{Other.m_triggerIntervalTime}
    , m_triggeredTimerHandle{Other.m_triggeredTimerHandle}
    , m_nameIndex{0}
    , m_repeatCount{Other.m_repeatCount.load()}
  {
  }

  FMDMeshCaptureShutter_IntervalTime& FMDMeshCaptureShutter_IntervalTime::operator=(const FMDMeshCaptureShutter_IntervalTime& Other) noexcept
  {
    if (this != &Other)
    {
      UnregisterTimer();
      m_meshCapturePtr = Other.m_meshCapturePtr;
      m_triggerIntervalTime = Other.m_triggerIntervalTime;
      m_triggeredTimerHandle = Other.m_triggeredTimerHandle;
      m_nameIndex.store(0, std::memory_order_relaxed);
      m_repeatCount.store(Other.m_repeatCount.load(), std::memory_order_relaxed);
    }

    return *this;
  }

  FMDMeshCaptureShutter_IntervalTime::FMDMeshCaptureShutter_IntervalTime(FMDMeshCaptureShutter_IntervalTime&& Other) noexcept
    : m_meshCapturePtr{Other.m_meshCapturePtr}
    , m_triggerIntervalTime{Other.m_triggerIntervalTime}
    , m_triggeredTimerHandle{Other.m_triggeredTimerHandle}
    , m_nameIndex{0}
    , m_repeatCount{Other.m_repeatCount.load()}
  {
    Other.m_meshCapturePtr = nullptr;
    Other.m_triggerIntervalTime = 0.0f;
    Other.m_triggeredTimerHandle.Invalidate();
    Other.m_nameIndex.store(0, std::memory_order_relaxed);
    Other.m_repeatCount.store(0, std::memory_order_relaxed);
  }

  FMDMeshCaptureShutter_IntervalTime& FMDMeshCaptureShutter_IntervalTime::operator=(FMDMeshCaptureShutter_IntervalTime&& Other) noexcept
  {
    if (this != &Other)
    {
      UnregisterTimer();
      m_meshCapturePtr = Other.m_meshCapturePtr;
      m_triggerIntervalTime = Other.m_triggerIntervalTime;
      m_triggeredTimerHandle = Other.m_triggeredTimerHandle;
      m_nameIndex.store(0, std::memory_order_relaxed);
      m_repeatCount.store(Other.m_repeatCount.load(), std::memory_order_relaxed);

      // Unregister timer before set rvalue to invalid
      Other.UnregisterTimer();
      Other.m_meshCapturePtr = nullptr;
      Other.m_triggeredTimerHandle.Invalidate();
      Other.m_nameIndex.store(0, std::memory_order_relaxed);
      Other.m_repeatCount.store(0, std::memory_order_relaxed);
    }

    return *this;
  }

  FMDMeshCaptureShutter_IntervalTime::~FMDMeshCaptureShutter_IntervalTime()
  {
    UnregisterTimer();
    m_meshCapturePtr = nullptr;
    m_repeatCount.store(0, std::memory_order_relaxed);
  }

  void FMDMeshCaptureShutter_IntervalTime::StartTriggerShutter(const int32 LODIndex,  const bool bShowImmediately)
  {
    // Only run this function in game thread
    if (! ::IsInGameThread())
    {
      return;
    }

    if (m_meshCapturePtr == nullptr)
    {
      return;
    }

    // If old timer is valid, unregister it
    if (m_triggeredTimerHandle.IsValid())
    {
      UnregisterTimer();
    }

    // Set up timer using TimerManager 
    UWorld* world = m_meshCapturePtr->GetWorld();
    if (world != nullptr)
    {
      FTimerManager& timeManager = world->GetTimerManager(); 
      // Remove current timer if handle is valid
      if (m_triggeredTimerHandle.IsValid())
      {
        timeManager.ClearTimer(m_triggeredTimerHandle);
      }

      // Callback of timer
      auto onTriggeredTimerCallback = [this, LODIndex, bShowImmediately]()
      {
        this->OnShutterTriggered(LODIndex, bShowImmediately);
      };

      // make it loop
      timeManager.SetTimer(m_triggeredTimerHandle, onTriggeredTimerCallback, m_triggerIntervalTime, true);
    }
  }

  void FMDMeshCaptureShutter_IntervalTime::SetShutterRepeatTimes(const int32 RepeatTimes)
  {
    m_repeatCount.store(RepeatTimes, std::memory_order_relaxed);
  }

  void FMDMeshCaptureShutter_IntervalTime::OnShutterTriggered(const int32 LODIndex, const bool bShowImmediately)
  {
    if (m_meshCapturePtr == nullptr)
    {
      return;
    }

    FMDMeshCaptureProxy* proxy = UMDMeshCapture::GetMeshCaptureProxyStatic<FMDMeshCaptureProxy>(m_meshCapturePtr);
    
    // Unregister timer if proxy is not valid or repeat is finished
    if ((proxy == nullptr) || (m_repeatCount.load() == 0))
    {
      UnregisterTimer();

      return;
    }

    // Note: Write FString and convert to FName
    FString snapshotNameBase = FString::Printf(TEXT("%s_%d"), *m_meshCapturePtr->GetCaptureName(), m_nameIndex++);

    FMDMeshSnapshot& targetSnapshot = proxy->AddSnapshot(FName(*snapshotNameBase));
    m_meshCapturePtr->SnapshotMesh(targetSnapshot, LODIndex);

    // Decrement repeat count if repeatCount it set to a positive value
    if (m_repeatCount.load() > 0)
    {
      const int32 newRepeatCount = m_repeatCount.load() - 1;
      m_repeatCount.store(newRepeatCount, std::memory_order_relaxed);

    }

    if (m_repeatCount.load() == 0)
    {
      OnRepeatFinished();
    }
  }

  void FMDMeshCaptureShutter_IntervalTime::UnregisterTimer()
  {
    if ((m_meshCapturePtr == nullptr) || !m_triggeredTimerHandle.IsValid())
    {
      return;
    }

    UWorld* world = m_meshCapturePtr->GetWorld();
    if (world == nullptr)
    {
      return;
    }

    // Unregister timer
    FTimerManager& timerManager = world->GetTimerManager();
    timerManager.ClearTimer(m_triggeredTimerHandle);
  }

  void FMDMeshCaptureShutter_IntervalTime::OnRepeatFinished()
  {
    // TODO Test code
    if (m_meshCapturePtr != nullptr)
    {
      m_meshCapturePtr->ShowSnapshots();
    }
  }
}