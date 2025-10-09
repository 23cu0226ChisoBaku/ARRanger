/**
 * @file ARPawnInitComponent.h
 * @brief Initialize component of ARRanger project 
 */

#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include <type_traits>

#include "ARPawnInitComponent.generated.h"

/**Forward declaration */
class APlayerState;
class UARAbilitySystemComponent;
class UARPawnInitData;
class UARChargeAttackComponent;

#define UE_API ARRANGER_API

/**
 * @class UARPawnInitComponent
 */
UCLASS( ClassGroup=(ARRanger), meta=(BlueprintSpawnableComponent) )
class UARPawnInitComponent : public UActorComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	/**
	 * @brief Default constructor
	 */
	UE_API UARPawnInitComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  template<typename PawnType>
  UE_API PawnType* GetPawn();

  template<typename PawnType>
  UE_API PawnType* GetPawnChecked();

  template<typename PlayerStateType>
  UE_API PlayerStateType* GetPlayerState();

  template<typename ControllerType>
  UE_API ControllerType* GetController();

  /**
   * @brief Find component in given actor.Return nullptr if not found.
   * @param InActor
   */
  UFUNCTION(BlueprintPure, Category = "ARRanger|Initialization")
  static UE_API UARPawnInitComponent* FindPawnInitComponent(const AActor* InActor);

  /**
   * @brief Get AbilitySystemComponent of ARRanger project 
   * @return UARAbilitySystemComponent*
   */
  UFUNCTION(BlueprintPure, Category = "GameplayAbility")
  UARAbilitySystemComponent* GetARAbilitySystemComponent() const { return AbilitySystemComponent; };

  /**Start IAbilitySystemInterface Interface */
  UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
  /**End IAbilitySystemInterface Interface */

  /**
   * @brief Get the Pawn Data object
   * 
   * @return const UARPawnInitData* 
   */
  const UARPawnInitData* GetPawnData() const { return PawnInitData; }

  /**
   * @brief Initialize AbilitySystemComponent
   * @param InASC         AbilitySystemComponent of ARRanger project
   * @param InOwnerActor  Owner actor
   */
  UE_API void InitializeAbilitySystem(UARAbilitySystemComponent* InASC, AActor* InOwnerActor);

  /**
   * @brief Initialize ARChargeAttackComponent
   * @param InCAC  ChargeAttackComponent
   */
  UE_API void InitializeChargeAttack(UARChargeAttackComponent* InCAC);

  /**
   * @brief Uninitialize AbilitySystemComponent
   */
  UE_API void UninitializeAbilitySystem();

  /**
   * @brief Uninitialize ChargeAttackComponent
   */
  UE_API void UninitializeChargeAttack();

protected:

  /**Start UActorComponent Interface */
  UE_API virtual void OnRegister() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/**End UActorComponent Interface */

private:

  UPROPERTY(VisibleAnywhere, Category = "PawnInit")
  TObjectPtr<UARAbilitySystemComponent> AbilitySystemComponent;

  UPROPERTY(VisibleAnywhere, Category = "PawnInit")
  TObjectPtr<UARChargeAttackComponent> ChargeAttackComponent;

  /**Asset to initialize pawn */
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