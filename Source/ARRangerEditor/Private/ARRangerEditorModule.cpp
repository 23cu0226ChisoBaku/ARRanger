#include "ARRangerEditorModule.h"
#include "PropertyEditorModule.h"
#include "CustomDetails/RangeDetectorCustomization.h"

#include "CustomDetails/RangeDetectorTargetCustomization.h"

IMPLEMENT_GAME_MODULE(FARRangerEditorModule, ARRangerEditor);

namespace
{
  static const FName PROPERTY_EDITOR_NAME{"PropertyEditor"};
}

void FARRangerEditorModule::StartupModule()
{
  // FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
  // propertyModule.RegisterCustomClassLayout(
  //   UTestUInterface::StaticClass()->GetFName(),
  //   FOnGetDetailCustomizationInstance::CreateStatic(&FRangeDetectorCustomization::MakeInstance)
  //   );

  RegisterPropertyTypeCustomizations();

  FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
  propertyModule.NotifyCustomizationModuleChanged();

}

void FARRangerEditorModule::ShutdownModule()
{
  if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
  {
    FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
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