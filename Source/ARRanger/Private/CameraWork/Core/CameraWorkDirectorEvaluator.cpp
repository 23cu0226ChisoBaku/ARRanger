#include "Core/CameraWorkDirectorEvaluator.h"

namespace AR
{
namespace CameraWork
{
  FCameraWorkDirectorEvaluator::FCameraWorkDirectorEvaluator()
    : m_director{nullptr}
  {

  }

  FCameraWorkDirectorEvaluator::~FCameraWorkDirectorEvaluator()
  {

  }

  const FCameraWorkDirector* FCameraWorkDirectorEvaluator::GetDirector() const
  {
    return m_director;
  }
  

  void FCameraWorkDirectorEvaluator::BuildDirectorEvaluator()
  {
    BuildDirectorEvaluatorImpl();
  }

  void FCameraWorkDirectorEvaluator::InitializeDirectorEvaluator()
  {
    InitializeDirectorEvaluatorImpl();
  }

  void FCameraWorkDirectorEvaluator::Evaluate()
  {
    EvaluateImpl();
  }

  void FCameraWorkDirectorEvaluator::BuildDirectorEvaluatorImpl()
  {
    // Empty implementation
  }

  void FCameraWorkDirectorEvaluator::InitializeDirectorEvaluatorImpl()
  {
    // Empty implementation 
  }

  void FCameraWorkDirectorEvaluator::SetDirector(const FCameraWorkDirector* Director)
  { 
    m_director = Director;
  }

}
}