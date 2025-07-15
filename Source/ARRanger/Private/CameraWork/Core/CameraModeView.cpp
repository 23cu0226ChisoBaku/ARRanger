#include "Core/CameraModeView.h"

namespace AR
{
namespace CameraWork
{
  FCameraModeView::FCameraModeView()
  {
    Reset();
  }

  void FCameraModeView::Reset()
  {
    Location = Private::Default<decltype(Location)>();

    Rotation = Private::Default<decltype(Rotation)>();
  }
}
}