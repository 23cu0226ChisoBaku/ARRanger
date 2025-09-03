/**
 * DEPRECATED
 */
#include "CustomDetails/RangeDetectorCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"

#include "Misc/ClassFilters/InterfaceClassFilter.h"
#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "ClassViewerModule.h"
#include "DetailWidgetRow.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "RangeDetectorCustomization"

TSharedRef<IDetailCustomization> FRangeDetectorCustomization::MakeInstance()
{
  return ::MakeShared<FRangeDetectorCustomization>();
}

void FRangeDetectorCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{ 
  m_targetInterfaceProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FDetectorTarget, TargetInterface), FDetectorTarget::StaticStruct());
  DetailBuilder.HideProperty(m_targetInterfaceProp);

  IDetailCategoryBuilder& categoryBuilder = DetailBuilder.EditCategory("TestUInterface");

  categoryBuilder.AddCustomRow(FText::FromString(TEXT("TargetClass")))
  [
    SNew(SComboButton)
    .OnGetMenuContent(this, &FRangeDetectorCustomization::MakeInterfacePickerWidget)
    .ButtonContent()
    [
      SNew(STextBlock)
      .Text(this, &FRangeDetectorCustomization::GetInterfacePropertyText)
    ]
  ];

}

TSharedRef<SWidget> FRangeDetectorCustomization::MakeInterfacePickerWidget()
{
  FClassViewerInitializationOptions options;
  options.Mode = EClassViewerMode::ClassPicker;
  options.ClassFilters.Emplace(FInterfaceClassFilter::MakeInstance());


  // FOnClassPicked
  // Same as TDelegate<void(UClass*)>
  return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(options, FOnClassPicked::CreateRaw(this, &FRangeDetectorCustomization::OnInterfacePicked));
}

void FRangeDetectorCustomization::OnInterfacePicked(UClass* ChosenClass)
{
  if (m_targetInterfaceProp.IsValid() && ChosenClass != nullptr)
  {
    m_targetInterfaceProp->SetValue(ChosenClass);
  }
}

FText FRangeDetectorCustomization::GetInterfacePropertyText() const
{
  UObject* value{nullptr};
  if (m_targetInterfaceProp.IsValid() && 
      m_targetInterfaceProp->GetValue(value) == FPropertyAccess::Success &&
      value != nullptr)
  {
    return FText::FromString(value->GetName());
  }

  return FText{};
}

#undef LOCTEXT_NAMESPACE