#pragma once

#ifndef _ARRANGER_EDITOR_INTERFACE_CLASS_FILTER_
#define _ARRANGER_EDITOR_INTERFACE_CLASS_FILTER_

#include "ClassViewerFilter.h"

class FInterfaceClassFilter : public IClassViewerFilter
{

public:

  static TSharedRef<IClassViewerFilter> MakeInstance();

  /**Start IClassViewerFilter interface */
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override;
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override;
  /**End IClassViewerFilter interface */
};


#endif // _ARRANGER_EDITOR_INTERFACE_CLASS_FILTER_