#include "CustomDetails/RangeDetectorTargetCustomization.h"

#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "Misc/ClassFilters/InterfaceClassFilter.h"

#include "Misc/Optional.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "ClassViewerModule.h"
#include "DetailWidgetRow.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "RangeDetectorTargetCustomization"

namespace
{
  static const FText TARGET_INTERFACE_ROW_TEXT = FText::FromString(TEXT("Target Interface"));
}

TSharedRef<IPropertyTypeCustomization> FRangeDetectorTargetCustomization::MakeInstance()
{
  return ::MakeShareable(new FRangeDetectorTargetCustomization{});
}

void FRangeDetectorTargetCustomization::CustomizeHeader(TSharedRef< IPropertyHandle > PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
  MakeHeaderRow(PropertyHandle, HeaderRow);
}

void FRangeDetectorTargetCustomization::CustomizeChildren(TSharedRef< IPropertyHandle > PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
  m_targetTypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDetectorTarget, TargetType));
  m_actorClassHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDetectorTarget, TargetActor));
  m_interfaceClassHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDetectorTarget, TargetInterface));

  ChildBuilder.AddProperty(m_targetTypeHandle.ToSharedRef());

  uint8 targetTypeEnumValue = 0;
  if (m_targetTypeHandle->GetValue(targetTypeEnumValue) == FPropertyAccess::Success)
  {
    EDetectorTargetType targetType = static_cast<EDetectorTargetType>(targetTypeEnumValue);

    if (targetType == EDetectorTargetType::Actor)
    {
      ChildBuilder.AddProperty(m_actorClassHandle.ToSharedRef());
    }
    else if (targetType == EDetectorTargetType::Interface)
    {
      ChildBuilder
      .AddCustomRow(TARGET_INTERFACE_ROW_TEXT)
      .NameContent()
      [
        m_interfaceClassHandle->CreatePropertyNameWidget()
      ]
      .ValueContent()
      .MaxDesiredWidth(0.0f)
      .MinDesiredWidth(125.0f)
      [
        SNew(SComboButton)
        .OnGetMenuContent(this, &FRangeDetectorTargetCustomization::MakeInterfacePickerWidget)
        .ButtonContent()
        [
          SNew(STextBlock)
          .Text(this, &FRangeDetectorTargetCustomization::GetInterfacePropertyText)
          .Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
        ]
      ];
    }
  }

  m_targetTypeHandle->SetOnPropertyValueChanged(
    TDelegate<void()>::CreateLambda
    (
      [&CustomizationUtils]()
      {
        CustomizationUtils.GetPropertyUtilities()->ForceRefresh();
      }
    )
  );
}

TSharedRef<SWidget> FRangeDetectorTargetCustomization::MakeInterfacePickerWidget()
{
  FClassViewerInitializationOptions options;
  options.Mode = EClassViewerMode::ClassPicker;
  options.ClassFilters.Emplace(FInterfaceClassFilter::MakeInstance());

  // FOnClassPicked
  // Same as TDelegate<void(UClass*)>
  return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(options, FOnClassPicked::CreateRaw(this, &FRangeDetectorTargetCustomization::OnInterfacePicked));
}

void FRangeDetectorTargetCustomization::OnInterfacePicked(UClass* ChosenClass)
{
  if (m_interfaceClassHandle.IsValid() && ChosenClass != nullptr)
  {
    m_interfaceClassHandle->SetValue(ChosenClass);
  }
}

FText FRangeDetectorTargetCustomization::GetInterfacePropertyText() const
{
  UObject* value{nullptr};
  if (m_interfaceClassHandle.IsValid() && 
      m_interfaceClassHandle->GetValue(value) == FPropertyAccess::Success &&
      value != nullptr)
  {
    return FText::FromString(value->GetName());
  }

  return FText{};
}

void FRangeDetectorTargetCustomization::MakeHeaderRow(TSharedRef<IPropertyHandle>& PropertyHandle, FDetailWidgetRow& HeaderRow)
{
  constexpr float MAX_PROPERTY_COUNT = 2.0f;

  TSharedPtr<SHorizontalBox> horizontalBox{};

  HeaderRow
  .NameContent()
  [
    PropertyHandle->CreatePropertyNameWidget()
  ]
  .ValueContent()
  .MaxDesiredWidth(125.0f * MAX_PROPERTY_COUNT)
  .MinDesiredWidth(125.0f * MAX_PROPERTY_COUNT)
  [
    SAssignNew(horizontalBox, SHorizontalBox)
    .IsEnabled(this, &FRangeDetectorTargetCustomization::IsValueEnabled, PropertyHandle.ToWeakPtr())
  ];
  
  
}

bool FRangeDetectorTargetCustomization::IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const
{
  if (WeakHandlePtr.IsValid())
  {
    return !WeakHandlePtr.Pin()->IsEditConst();
  }

  return false;
}

#undef LOCTEXT_NAMESPACE