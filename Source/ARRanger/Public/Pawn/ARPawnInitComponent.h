// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"

#include <type_traits>

#include "ARPawnInitComponent.generated.h"

class APlayerState;
class UARAbilitySystemComponent;
class UARPawnInitData;
class UARChargeAttackComponent;

#define UE_API ARRANGER_API

UCLASS( ClassGroup=(ARRanger), meta=(BlueprintSpawnableComponent) )
class UARPawnInitComponent : public UActorComponent, public IAbilitySystemInterface
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
  static UE_API UARPawnInitComponent* FindPawnInitComponent(const AActor* InActor);

  UFUNCTION(BlueprintPure, Category = "GameplayAbility")
  UARAbilitySystemComponent* GetARAbilitySystemComponent() const { return AbilitySystemComponent; };

  UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

  const UARPawnInitData* GetPawnData() const { return PawnInitData; }

  UE_API void InitializeAbilitySystem(UARAbilitySystemComponent* InASC, AActor* InOwnerActor);

  UE_API void InitializeChargeAttack(UARChargeAttackComponent* InCAC);

  UE_API void UninitializeAbilitySystem();

  UE_API void UninitializeChargeAttack();

protected:

  /**Start UActorComponent Interface */
  UE_API virtual void OnRegister() override;
  UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/**End UActorComponent Interface */

private:

  UPROPERTY(VisibleAnywhere, Category = "PawnInit")
  TObjectPtr<UARAbilitySystemComponent> AbilitySystemComponent;

  UPROPERTY(VisibleAnywhere, Category = "PawnInit")
  TObjectPtr<UARChargeAttackComponent> ChargeAttackComponent;

  UPROPERTY(EditDefaultsOnly, Category = "PawnInit", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<const UARPawnInitData> PawnInitData;

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