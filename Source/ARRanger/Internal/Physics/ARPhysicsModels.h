#pragma once

#ifndef _AR_PHYSICS_CORE_MODELS_
#define _AR_PHYSICS_CORE_MODELS_

namespace ARRanger
{

namespace Physics
{
  /**
   * @brief 磁力パラメータ
   */
  struct FMagneticParameters
  {
    /**磁力オブジェクト磁極の強さ */
    float MagneticCharge;

    /**磁力オブジェクトの重さ */
    float MagneticObjectMass;
  };

} // namespace ARRanger::Physics

} // namespace ARRanger


#endif // _AR_PHYSICS_CORE_MODELS_