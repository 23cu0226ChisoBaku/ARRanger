#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_EVALUATOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_EVALUATOR_

#include "Core/CoreDefines.h"

#include "Misc/EnvironmentTypeAdapter.h"

#include <type_traits>

namespace AR
{
namespace CameraWork
{
  class FCameraWorkDirector;

  class FCameraWorkDirectorEvaluator
  {
    friend FCameraWorkDirector;

    public:
      MCW_API FCameraWorkDirectorEvaluator();
      MCW_API ~FCameraWorkDirectorEvaluator();

      const FCameraWorkDirector* GetDirector() const;
      
      template<typename DirectorType>
      const DirectorType* GetDirectorAs() const;

      MCW_API void BuildDirectorEvaluator();

      MCW_API void InitializeDirectorEvaluator();

      MCW_API void Evaluate();

    protected:

      virtual void BuildDirectorEvaluatorImpl();

      virtual void InitializeDirectorEvaluatorImpl();

      virtual void EvaluateImpl() = 0;

    private:
      void SetDirector(const FCameraWorkDirector* Director);

    private:
      const FCameraWorkDirector* m_director;
  };

  template<typename DirectorType>
  const DirectorType* FCameraWorkDirectorEvaluator::GetDirectorAs() const
  {
    return CastFast<DirectorType>(m_director);
  }

  template<typename DirectorType>
  class TCameraWorkDirectorEvaluator : public FCameraWorkDirectorEvaluator
  {
    static_assert(std::is_base_of_v<FCameraWorkDirector, DirectorType>, "Use non devired class of FCameraWorkDirector is invalid");

    public:
      const DirectorType* GetDirector() const;
  };
}
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_EVALUATOR_