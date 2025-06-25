// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef MLIB_GETADDCOMP
#define MLIB_GETADDCOMP

#include "CoreMinimal.h"

#include <type_traits>

MLIBRARY_API DECLARE_LOG_CATEGORY_EXTERN(MLibrary_GetAddComp_Error, Log, All);

namespace MLibrary
{
  class MLIBRARY_API GetAddComponent
  {
  public:
    template<typename UnrealComponent>
    static UnrealComponent* GetComponent(AActor* actor)
    {
      static_assert(sizeof(UnrealComponent) > 0, "Can't use incomplete Type");
      static_assert(std::is_convertible<UnrealComponent*, UActorComponent*>::value, "Can't convert to UActorComponent");

      if (!::IsValid(actor))
      {
        UE_LOG(MLibrary_GetAddComp_Error, Error, TEXT("Actor is Invalid"));
        return nullptr;
      }
      
      return actor->FindComponentByClass<UnrealComponent>();
    }

    template<typename UnrealComponent>
    static UnrealComponent* AddComponent(AActor* actor, FName CompName = TEXT(""))
    {
      static_assert(sizeof(UnrealComponent) > 0, "Can't use incomplete Type");
      static_assert(std::is_convertible<UnrealComponent*, UActorComponent*>::value, "Can't convert to UActorComponent");

      if (!::IsValid(actor))
      {
        UE_LOG(MLibrary_GetAddComp_Error, Error, TEXT("Actor is Invalid"));
        return nullptr;
      }
      
      return actor->CreateDefaultSubobject<UnrealComponent>(CompName);
    }

    template<typename UnrealComponent>
    static UnrealComponent* GetOrAddComponent(AActor* actor)
    {
      static_assert(sizeof(UnrealComponent) > 0, "Can't use incomplete Type");
      static_assert(std::is_convertible<UnrealComponent*, UActorComponent*>::value, "Can't convert to UActorComponent");

      UnrealComponent* comp = GetComponent(actor);

      if (comp == nullptr)
      {
        comp = AddComponent(actor);
      }

      return comp;
    }
  };
}

#define GetComponent(ActorPtr, ComponentType) 			MLibrary::GetAddComponent::GetComponent<ComponentType>(ActorPtr);
#define AddComponent(ActorPtr, ComponentType) 			MLibrary::GetAddComponent::AddComponent<ComponentType>(ActorPtr);
#define AddComponent_Name(ActorPtr, ComponentType, Name) MLibrary::GetAddComponent::AddComponent<ComponentType>(ActorPtr, Name);
#define GetOrAddComponent(ActorPtr, ComponentType) 	MLibrary::GetAddComponent::GetOrAddComponent<ComponentType>(ActorPtr);

#endif