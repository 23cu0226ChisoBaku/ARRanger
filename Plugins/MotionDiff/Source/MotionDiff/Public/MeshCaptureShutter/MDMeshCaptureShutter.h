// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <atomic>

class UMDMeshCapture;

namespace MotionDiff
{
  namespace EShutterTriggerMode

  {
    enum
    {
      Infinity = -1,
    };

  }
}


namespace MotionDiff
{
  struct FMDMeshCaptureShutter
  {
    private:
      struct IShutterConcept
      {
        virtual ~IShutterConcept() = default;
        virtual void StartTriggerShutter(const int32 LODIndex, const bool bShowImmediately) = 0;
      };

      template<typename ShutterType>
      struct ShutterModel final: public IShutterConcept
      {
        ShutterModel(ShutterType ShutterInstance)
          : m_instance{::MoveTemp(ShutterInstance)}
        {
        }

        // Start of IShutterConcept
        void StartTriggerShutter(const int32 LODIndex, const bool bShowImmediately) override final
        {
          m_instance.StartTriggerShutter(LODIndex, bShowImmediately);
        }
        // End of IShutterConcept

        private:
          ShutterType m_instance;
      };

    public:
      template<typename ShutterType>
      FMDMeshCaptureShutter(ShutterType ShutterInstance)
        : m_pimpl{::MakeUnique<ShutterModel<ShutterType>>(ShutterInstance)}
      {
      }

      void StartTriggerShutter(const int32 LODIndex = 0, const bool bShowImmediately = true)
      {
        m_pimpl->StartTriggerShutter(LODIndex, bShowImmediately);
      }

    private:
      TUniquePtr<IShutterConcept> m_pimpl;
  };

  struct FMDMeshCaptureShutter_IntervalTime
  {
    FMDMeshCaptureShutter_IntervalTime(UMDMeshCapture* MeshCapture, const float TriggerInterval);
    FMDMeshCaptureShutter_IntervalTime(const FMDMeshCaptureShutter_IntervalTime& Other) noexcept;
    FMDMeshCaptureShutter_IntervalTime& operator=(const FMDMeshCaptureShutter_IntervalTime& Other) noexcept;
    FMDMeshCaptureShutter_IntervalTime(FMDMeshCaptureShutter_IntervalTime&& other) noexcept;
    FMDMeshCaptureShutter_IntervalTime& operator=(FMDMeshCaptureShutter_IntervalTime&& Other) noexcept;
    ~FMDMeshCaptureShutter_IntervalTime();
    
    // NOTE: Start static-polymorphism
    public:
      void StartTriggerShutter(const int32 LODIndex, const bool bShowImmediately);
    // NOTE: End static-polymorphism

    public:
      void SetShutterRepeatTimes(const int32 RepeatTimes);

    private:
      void OnShutterTriggered(const int32 LODIndex, const bool bShowImmediately);
      void OnRepeatFinished();
      void UnregisterTimer();

    private:
      UMDMeshCapture* m_meshCapturePtr;

      // unit : second
      float m_triggerIntervalTime;

      FTimerHandle m_triggeredTimerHandle;

      std::atomic<int32> m_nameIndex;

      std::atomic<int32> m_repeatCount;

  };

}
