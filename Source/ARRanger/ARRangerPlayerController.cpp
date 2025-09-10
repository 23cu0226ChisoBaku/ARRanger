// Copyright Epic Games, Inc. All Rights Reserved.


#include "ARRangerPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Input/ARInputMappingContext.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "Input/ARInputComponent.h"
#include "Input/ARPlayerInputBuffer.h"

#include "Internal/ARLoggingHeader.h"
#include "ARGameplayTags.h"

namespace
{
  const FString DEFAULT_IMC_TAG_NAME = TEXT("InputState_Default");  
}

UARAbilitySystemComponent* AARRangerPlayerController::GetARASC() const
{
  return UARAbilitySystemComponent::FindARAbilitySystemComponent(GetOwner());
}


void AARRangerPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
  Super::PostProcessInput(DeltaTime, bGamePaused);

  EvaluateInputBuffer(DeltaTime, bGamePaused);

  UARAbilitySystemComponent* ASC = GetARASC();
  if (ASC != nullptr)
  {
    FARAbilityInputProcessParameter inputProcessParam{};
    inputProcessParam.DeltaTime = DeltaTime;
    inputProcessParam.bGamePaused = bGamePaused;
    ASC->ProcessAbilityInputs(inputProcessParam);
  }

}

void AARRangerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Contexts
  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
    Subsystem->ClearAllMappings();

    FGameplayTag defaultIMCTag = ARRanger::GameplayTags::FindExactTagByString(DEFAULT_IMC_TAG_NAME);
    SwitchNextIMC(defaultIMCTag);
  }

  InitializePlayerInput();
}

void AARRangerPlayerController::OnGameplayAbilityActivate(const FGameplayTag& InNextIMCTag)
{
  if (InputMappingContext == nullptr)
  {
    return;
  }

  SwitchNextIMC(InNextIMCTag);
  
}

void AARRangerPlayerController::OnGameplayAbilityEnd(bool bWasCanceled)
{
  if (InputMappingContext == nullptr)
  {
    return;
  }

  // Return to default IMC if GA ends normally
  if (!bWasCanceled)
  {
    FGameplayTag defaultIMCTag = ARRanger::GameplayTags::FindExactTagByString(DEFAULT_IMC_TAG_NAME);
    SwitchNextIMC(defaultIMCTag);
  }
}

void AARRangerPlayerController::SwitchNextIMC(const FGameplayTag& InNextIMCTag)
{
  UInputMappingContext* defaultIMC = InputMappingContext->FindIMCWithTag(InNextIMCTag);
  if (defaultIMC != nullptr && CurrentIMC != defaultIMC)
  {
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem != nullptr)
    {
      Subsystem->RemoveMappingContext(CurrentIMC);
      Subsystem->AddMappingContext(defaultIMC, 0);
  
      CurrentIMC = defaultIMC;
    }
  }
}

void AARRangerPlayerController::InitializePlayerInput()
{
  if (InputConfig == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("Invalid Player Input Config. InitializePlayerInput will not call"));
    return;
  }

  UARInputComponent* ARIC = ::Cast<UARInputComponent>(InputComponent);
  check(ARIC);
  if (ARIC == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("Unexpected input component class. Abilities will not be bound to their inputs.Use UARInputComponent or its subclass"));
    return;
  }

  // Bind input action to gameplay tag;
  ARIC->BindAbilityActions(InputConfig, this, &AARRangerPlayerController::AbilityInputTagPressed, &AARRangerPlayerController::AbilityInputTagReleased, m_bindHandles);

  // Initialize Input Buffer
  InitializePlayerInputBuffer(ARIC);
  
}

void AARRangerPlayerController::InitializePlayerInputBuffer(UARInputComponent* InInputComponent)
{
  check(InInputComponent != nullptr);
  if (InputBufferClass != nullptr)
  {
    InputBuffer = ::NewObject<UARPlayerInputBuffer>(/**Outer */this, InputBufferClass);
    check(InputBuffer != nullptr);
  }
}

void AARRangerPlayerController::AbilityInputTagPressed(FGameplayTag InInputTag)
{
  // If buffer is invalid then handle input,otherwise use input buffer to handle input 
  if (InputBuffer == nullptr)
  {
    if (UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(GetOwner()))
    {
      ARASC->AbilityInputTagPressed(InInputTag);
    }
  }
}

void AARRangerPlayerController::AbilityInputTagReleased(FGameplayTag InInputTag)
{
  // If buffer is invalid then handle input,otherwise use input buffer to handle input 
  if (InputBuffer == nullptr)
  {
    if (UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(GetOwner()))
    {
      ARASC->AbilityInputTagReleased(InInputTag);
    }
  }
}

void AARRangerPlayerController::EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused)
{
  if (InputBuffer != nullptr)
  {
    InputBuffer->EvaluateBuffer(this, DeltaTime, bGamePaused);
  }
}
