// Fill out your copyright notice in the Description page of Project Settings.
/*
Module MLibrary

UInterfaceValidCheck.h

Author : MAI ZHICONG

Description : Check UInterface is valid
Support verion: 5.3.2

Update History: 2024/12/16 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#include "CoreMinimal.h"

#ifndef M_LIB_UINTERFACE_VALID
#define M_LIB_UINTERFACE_VALID

#define IS_UINTERFACE_VALID(interface)  ((interface != nullptr) && (::IsValid(interface->_getUObject())))
                                      

#endif