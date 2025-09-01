#pragma once

#ifndef _ARRANGER_EDITOR_RANGE_DETECTOR_CUSTOMIZATION_
#define _ARRANGER_EDITOR_RANGE_DETECTOR_CUSTOMIZATION_

#include "IDetailCustomization.h"

/**
 * DEPRECATED
 */
class FRangeDetectorCustomization : public IDetailCustomization
{
  public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    /**Start IDetailCustomization interface */
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    /**End IDetailCustomization interface */

  private:

    TSharedRef<SWidget> MakeInterfacePickerWidget();

    void OnInterfacePicked(UClass* ChosenClass);
    FText GetInterfacePropertyText() const;

  private:
    TSharedPtr<IPropertyHandle> m_targetInterfaceProp;
};


#endif // _ARRANGER_EDITOR_RANGE_DETECTOR_CUSTOMIZATION_