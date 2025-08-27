#include "ARRangerEditorModule.h"
#include "PropertyEditorModule.h"
#include "CustomDetails/RangeDetectorCustomization.h"
#include "Test/TestUInterface.h"


IMPLEMENT_GAME_MODULE(FARRangerEditorModule, ARRangerEditor);

void FARRangerEditorModule::StartupModule()
{
  FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
  propertyModule.RegisterCustomClassLayout(
    UTestUInterface::StaticClass()->GetFName(),
    FOnGetDetailCustomizationInstance::CreateStatic(&FRangeDetectorCustomization::MakeInstance)
    );
}

void FARRangerEditorModule::ShutdownModule()
{
  if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
  {
    FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    propertyModule.UnregisterCustomClassLayout(UTestUInterface::StaticClass()->GetFName());
  }
}