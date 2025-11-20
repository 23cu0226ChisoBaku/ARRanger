// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "INotifyHandlerInterface.h"

#include "IObservableSubjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UObservableSubjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IObservableSubjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  TSharedRef<ARRanger::INotifyHandlerInterface> GetNotifyHandlerRef() const
  {
    return m_notifyHandler.ToSharedRef();
  }

  void UpdateNotifyHandler(const TSharedPtr<ARRanger::INotifyHandlerInterface>& NewNotifyHandler)
  {
    m_notifyHandler = NewNotifyHandler;
  }

private:
  TSharedPtr<ARRanger::INotifyHandlerInterface> m_notifyHandler = nullptr;

};
