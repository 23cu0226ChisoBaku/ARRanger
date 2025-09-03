#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"


class FARRangerEditorModule : public IModuleInterface
{
  public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

  private:
    void RegisterPropertyTypeCustomizations();

    void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);

  private:
    TSet< FName > m_registeredPropertyTypes;
};