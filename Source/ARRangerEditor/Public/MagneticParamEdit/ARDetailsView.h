// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DetailsView.h"
#include "ARDetailsView.generated.h"



UCLASS()
class ARRANGEREDITOR_API UARDetailsView : public UDetailsView
{
	GENERATED_BODY()
	
	/**Start FNotifyHook interface */
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	/**End FNotifyHook interface */

public:

  UPROPERTY(BlueprintAssignable, Category = "View|Advanced|Event")
  FOnPropertyValueChanged OnPropertyChanged_Advanced;

};
