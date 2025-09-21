// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "ARHealthComponent.generated.h"

#define UE_API ARRANGER_API

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API UARHealthComponent();

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedDelegate, UARHealthComponent*, HealthComponent, AActor*, Instigator, float, OldHealthValue, float, NewHealthValue);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadDelegate, AActor*, OwningActor);

  UPROPERTY(BlueprintAssignable)
  FOnHealthChangedDelegate OnHealthChanged;

  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnDead;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetMaxHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealthNormalized() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API bool IsDead() const;

  UE_API void HandleHealthChange(AActor* Instigator, float ChangeValue);

protected:

  /**Start UActorComponent Interface */
	UE_API virtual void BeginPlay() override;
  /**End UActorComponent Interface */

private:
  void SetHealthInternal(float NewHealth);
  void SetMaxHealthInternal(float NewMaxHealth);

private:
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Health", meta = (AllowPrivateAccess = "true"))
  float MaxHealth;

  UPROPERTY(VisibleAnywhere, Category = "ARRanger|Health")
  float Health;
		
};

#undef UE_API