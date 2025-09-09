// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"

#include <type_traits>

#include "ARPawnInitComponent.generated.h"

class APlayerState;
class UInputComponent;
class UARPlayerInputBuffer;

#define UE_API ARRANGER_API

UCLASS( ClassGroup=(ARRanger), meta=(BlueprintSpawnableComponent) )
class UARPawnInitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API UARPawnInitComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  template<typename PawnType>
  UE_API PawnType* GetPawn();

  template<typename PawnType>
  UE_API PawnType* GetPawnChecked();

  template<typename PlayerStateType>
  UE_API PlayerStateType* GetPlayerState();

  template<typename ControllerType>
  UE_API ControllerType* GetController();

  UFUNCTION(BlueprintPure, Category = "ARRanger|Initialization")
  static UARPawnInitComponent* FindPawnInitComponent(const APawn* InPawn);

protected:

  /**Start UActorComponent Interface */
  UE_API virtual void OnRegister() override;
  UE_API virtual void BeginPlay() override;
	/**End UActorComponent Interface */

private:
  void InitializeASC();

  void InitializePlayerInput();

  void InitializePlayerInput(UInputComponent* InPlayerInputComponent, UARPlayerInputBuffer* PlayerInputBuffer = nullptr);

private:

  TArray<uint32> BindInputHandles; 
};


template<typename PawnType>
PawnType* UARPawnInitComponent::GetPawn()
{
  static_assert(std::is_base_of_v<APawn, PawnType>, "Invalid template parameter of GetPawn. Must be derived from APawn");
  return ::Cast<PawnType>(GetOwner());
}

template<typename PawnType>
PawnType* UARPawnInitComponent::GetPawnChecked()
{
  static_assert(std::is_base_of_v<APawn, PawnType>, "Invalid template parameter of GetPawnChecked. Must be derived from APawn");
  return ::CastChecked<PawnType>(GetOwner());
}

template<typename PlayerStateType>
PlayerStateType* UARPawnInitComponent::GetPlayerState()
{
  static_assert(std::is_base_of_v<APlayerState, PlayerStateType>, "Invalid template parameter of GetPlayerState. Must be derived from APlayerState");
  return GetPawnChecked<APawn>()->GetPlayerState<PlayerStateType>();
}

template<typename ControllerType>
ControllerType* UARPawnInitComponent::GetController()
{
  static_assert(std::is_base_of_v<AController, ControllerType>, "Invalid template parameter of GetController. Must be derived from AController");
  return GetPawnChecked<APawn>()->GetController<ControllerType>();
}


#undef UE_API