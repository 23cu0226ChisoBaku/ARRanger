// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticParamEdit/ARDetailsView.h"

void UARDetailsView::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
  Super::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);

  // Use GetMemberPropertyName to get actual member name?
  // TODO Maybe not
  OnPropertyChanged_Advanced.Broadcast(PropertyChangedEvent.GetMemberPropertyName());
}