// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

namespace ARRanger
{

namespace Repository
{
  template<typename RepositoryIDType, typename DataType>
  class IARRepositoryGateway;
} // namespace ARRanger::Repository

namespace Physics
{
  struct FMagneticParameterDTO;
} // namespace ARRanger::Physics

} // namespace ARRanger

class UObject;

/**
 *  On screen widget to change magnetic param
 */
class SMagneticParamEditPanelWidget : public SCompoundWidget
{

  using RepositoryType = ARRanger::Repository::IARRepositoryGateway<UObject*, ARRanger::Physics::FMagneticParameterDTO>;
  using RepositoryTypePtr = RepositoryType*;

public:

  SLATE_BEGIN_ARGS(SMagneticParamEditPanelWidget) {}

  SLATE_ARGUMENT(RepositoryTypePtr, RepositoryConnection)

  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs);

  virtual bool SupportsKeyboardFocus() const override { return true; }

  void SetRepository(RepositoryTypePtr InRepositoryConnection);
  void ResetRepository();

private:
  /**Connection to repository */
  RepositoryTypePtr RepositoryConnection;

};
