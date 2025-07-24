#include "Core/CameraWorkDirector.h"
#include "Core/CameraWorkDirectorEvaluator.h"

namespace AR
{
namespace CameraWork
{
  FCameraWorkDirector::FCameraWorkDirector()
  { }

  FCameraWorkDirector::~FCameraWorkDirector()
  { }

  FCameraWorkDirectorEvaluatorPtr FCameraWorkDirector::MakeDirectorEvaluator() const
  {
    FCameraWorkDirectorEvaluatorPtr directorEvaluator = MakeDirectorEvaluatorImpl();
    if (directorEvaluator != nullptr)
    {
      directorEvaluator->SetDirector(this);
    }

    return directorEvaluator;
  }

  FCameraWorkDirectorEvaluatorPtr FCameraWorkDirector::MakeDirectorEvaluatorImpl() const
  {
    return nullptr;
  }
}
}