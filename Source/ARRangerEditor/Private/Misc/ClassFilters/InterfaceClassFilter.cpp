#include "Misc/ClassFilters/InterfaceClassFilter.h"

TSharedRef<IClassViewerFilter> FInterfaceClassFilter::MakeInstance()
{
  return ::MakeShared<FInterfaceClassFilter>();
}

bool FInterfaceClassFilter::IsClassAllowed( const FClassViewerInitializationOptions& InInitOptions, 
                                            const UClass* InClass, 
                                            TSharedRef< FClassViewerFilterFuncs > InFilterFuncs )
{
  /**
   * ImplementsInterface will IGNORE UInterface::StaticClass()
   * @see UClass::ImplementsInterface
   */
  return InClass->IsChildOf(UInterface::StaticClass());
}

bool FInterfaceClassFilter::IsUnloadedClassAllowed( const FClassViewerInitializationOptions& InInitOptions, 
                                                    const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, 
                                                    TSharedRef< FClassViewerFilterFuncs > InFilterFuncs)
{
  return false;
}