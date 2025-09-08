#include "ARRangerEditorModule.h"
#include "PropertyEditorModule.h"
#include "CustomDetails/RangeDetectorCustomization.h"

#include "CustomDetails/RangeDetectorTargetCustomization.h"

#include "RangeDetector/GameFramework/RangeDetectorComponent.h"
#include "CustomComponentVisualizer/ARRangeDetectorComponentVisualizer.h"

#include "UnrealEdGlobals.h"        // Access GUnrealEd
#include "Editor/UnrealEdEngine.h"

IMPLEMENT_GAME_MODULE(FARRangerEditorModule, ARRangerEditor);

#define LOCTEXT_NAMESPACE "FARRangerEditorModule"

#define REGISTER_CUSTOM_COMPONENT_VISUALIZER(ActorComponentType, ComponentVisualizerSharedPtr) \
  this->RegisterCustomComponentVisualizer(ActorComponentType::StaticClass()->GetFName(), ComponentVisualizerSharedPtr)

namespace
{
  static const FName PROPERTY_EDITOR_NAME{"PropertyEditor"};
}

void FARRangerEditorModule::StartupModule()
{
  RegisterComponentVisualizers();

  RegisterPropertyTypeCustomizations();

  FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
  propertyModule.NotifyCustomizationModuleChanged();

}

void FARRangerEditorModule::ShutdownModule()
{
  if (FModuleManager::Get().IsModuleLoaded(PROPERTY_EDITOR_NAME))
  {
    FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
    for (const auto& propertyTypeName : m_registeredPropertyTypes)
    {
      propertyModule.UnregisterCustomPropertyTypeLayout(propertyTypeName);
    }

    propertyModule.NotifyCustomizationModuleChanged();
  }


}

void FARRangerEditorModule::RegisterPropertyTypeCustomizations()
{
  RegisterCustomPropertyTypeLayout("DetectorTarget", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRangeDetectorTargetCustomization::MakeInstance));
}

void FARRangerEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
  check(PropertyTypeName != NAME_None);

  m_registeredPropertyTypes.Add(PropertyTypeName);
  FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
  propertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

void FARRangerEditorModule::RegisterComponentVisualizers()
{
  REGISTER_CUSTOM_COMPONENT_VISUALIZER(URangeDetectorComponent, FARRangeDetectorComponentVisualizer::MakeInstance());
}

void FARRangerEditorModule::RegisterCustomComponentVisualizer(FName ClassName, TSharedPtr<FComponentVisualizer> VisualizerPtr)
{
  check(VisualizerPtr.IsValid());
  check(ClassName != NAME_None);

  if (GUnrealEd != nullptr)
  {
    m_registeredComponentVisualizerTypes.Add(ClassName);
    GUnrealEd->RegisterComponentVisualizer(ClassName, VisualizerPtr);
    VisualizerPtr->OnRegister();
  }
}

void FARRangerEditorModule::UnregisterComponentVisualizers()
{
  if (GUnrealEd != nullptr)
  {
    for (const FName& registeredName : m_registeredComponentVisualizerTypes)
    {
      GUnrealEd->UnregisterComponentVisualizer(registeredName);
    }
  }
}

#undef LOCTEXT_NAMESPACE
#undef REGISTER_CUSTOM_COMPONENT_VISUALIZER