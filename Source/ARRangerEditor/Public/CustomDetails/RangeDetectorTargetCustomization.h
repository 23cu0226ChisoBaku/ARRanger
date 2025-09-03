#pragma once

#ifndef _ARRANGER_EDITOR_RANGE_DETECTOR_TARGET_CUSTOMIZATION_
#define _ARRANGER_EDITOR_RANGE_DETECTOR_TARGET_CUSTOMIZATION_

// Customization for USTRUCT
#include "IPropertyTypeCustomization.h"

class FRangeDetectorTargetCustomization : public IPropertyTypeCustomization
{

  public:

    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /**Start IPropertyTypeCustomization interface */
    virtual void CustomizeHeader(TSharedRef< IPropertyHandle > PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
    virtual void CustomizeChildren(TSharedRef< IPropertyHandle > PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
    /**End IPropertyTypeCustomization interface */

  private:
    
    TSharedRef<SWidget> MakeInterfacePickerWidget();
    void OnInterfacePicked(UClass* ChosenClass);
    FText GetInterfacePropertyText() const;

    void MakeHeaderRow(TSharedRef<IPropertyHandle>& PropertyHandle, FDetailWidgetRow& HeaderRow);
    bool IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const;

  private:

    TSharedPtr<IPropertyHandle> m_targetTypeHandle;
    TSharedPtr<IPropertyHandle> m_actorClassHandle;
    TSharedPtr<IPropertyHandle> m_interfaceClassHandle;

};

#endif // _ARRANGER_EDITOR_RANGE_DETECTOR_TARGET_CUSTOMIZATION_