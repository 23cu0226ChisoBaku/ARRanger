#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FComponentVisualizer;


class FARRangerEditorModule : public IModuleInterface
{
  public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

  private:
    void RegisterPropertyTypeCustomizations();

    void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);

    void RegisterComponentVisualizers();

    void RegisterCustomComponentVisualizer(FName ClassName, TSharedPtr<FComponentVisualizer> VisualizerPtr);

    void UnregisterComponentVisualizers();

    void ExtendAnimEditor();

  private:
    TSet< FName > m_registeredPropertyTypes;

    TSet< FName > m_registeredComponentVisualizerTypes; 

};