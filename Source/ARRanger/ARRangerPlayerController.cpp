// Copyright Epic Games, Inc. All Rights Reserved.


#include "ARRangerPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Input/ARInputMappingContext.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "Input/ARInputComponent.h"
#include "Input/ARPlayerInputBuffer.h"
#include "InputActionValue.h"
#include "Player/ARPlayerState.h"
#include "PlayerComponents/ARChargeAttackComponent.h"
#include "Pawn/ARPawnInitComponent.h"
// TODO
#include "ARRangerCharacter.h"

#include "Internal/ARLoggingHeader.h"
#include "ARGameplayTags.h"

#include <atomic>

namespace
{
  const FString DEFAULT_IMC_TAG_NAME = TEXT("InputState.Default");  
  static std::atomic<int32> gHoldHandle = INDEX_NONE + 1;
}

const FGA_HoldHandle FGA_HoldHandle::InvalidHandle{};

bool FGA_HoldHandle::IsValid() const
{
  return *this != InvalidHandle;
}


FGA_HoldHandle::FGA_HoldHandle()
: m_handleID{INDEX_NONE}
{ }

void FGA_HoldHandle::GenerateNewHandle()
{
  // Try to generate
  m_handleID = gHoldHandle++;
  
  // Make sure it will not generate InvalidHandle
  if (!IsValid())
  {
    GenerateNewHandle();
  }
  
  check(IsValid());
}

void FGA_HoldHandle::Reset()
{
  m_handleID = INDEX_NONE;
}

bool operator==(const FGA_HoldHandle& Lhs, const FGA_HoldHandle& Rhs)
{
  return Lhs.m_handleID == Rhs.m_handleID;
}

bool operator!=(const FGA_HoldHandle& Lhs, const FGA_HoldHandle& Rhs)
{
  return !(Lhs == Rhs);
}

bool AARRangerPlayerController::FHoldSpec::IsValid() const
{
  return Handle.IsValid();
}

bool AARRangerPlayerController::FHoldSpec::operator==(const AARRangerPlayerController::FHoldSpec& Other) const
{
  return Handle == Other.Handle;
}

bool AARRangerPlayerController::FHoldSpec::operator!=(const AARRangerPlayerController::FHoldSpec& Other) const
{
  return !(*this == Other);
}

void AARRangerPlayerController::BeginPlay()
{
  Super::BeginPlay();

  if (AARPlayerState* ARPS = GetPlayerState<AARPlayerState>())
  {
    ARPS->OnPlayerStateInitialized(this);
  }

  if (AARRangerCharacter* character = ::Cast<AARRangerCharacter>(GetPawn()))
  {
    OwningCharacter = character;
  }
}

UARAbilitySystemComponent* AARRangerPlayerController::GetARASC() const
{
  return UARAbilitySystemComponent::FindARAbilitySystemComponent(GetPawn());
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

void AARRangerPlayerController::OnPossess(APawn* InPawn)
{
  Super::OnPossess(InPawn);

  if (AARPlayerState* ARPS = GetPlayerState<AARPlayerState>())
  {
    if (UARPawnInitComponent* PIC = ::Cast<UARPawnInitComponent>(InPawn->GetComponentByClass(UARPawnInitComponent::StaticClass())))
    {
      PIC->InitializeAbilitySystem(ARPS->GetARAbilitySystemComponent(), ARPS); 
  
      PIC->InitializeChargeAttack(ARPS->GetARChargeAttackComponent());
    }
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

FGABlueprintableHoldHandle AARRangerPlayerController::OnGameplayAbilityActivated_Hold(FGameplayTag InActivatedAbilityTag, bool bBlockInputTag, FGameplayTagContainer InInputBlockIgnoreTags)
{
  if (OwningCharacter != nullptr)
  {
    OwningCharacter->OnHoldStarted(InActivatedAbilityTag);
  }
  
  if (bBlockInputTag)
  {
    FHoldSpec newHoldSpec{};
    newHoldSpec.Handle.GenerateNewHandle();
    newHoldSpec.InputBlockIgnoreTags = InInputBlockIgnoreTags;

    m_holdSpecs.Emplace(newHoldSpec);

    check(newHoldSpec.Handle.IsValid());
    return FGABlueprintableHoldHandle{newHoldSpec.Handle};
  }


  return FGABlueprintableHoldHandle{};

}

void AARRangerPlayerController::OnGameplayAbilityTaskTicked_Holding(FGameplayTag InTaskOwnerAbilityTag, float HeldTime, float DeltaTime)
{
  // TODO Try use UARChargeAttackComponent in ARRangerCharacter
  if (AARPlayerState* ARPS = GetPlayerState<AARPlayerState>())
  {
    if (UARChargeAttackComponent* CAC = ARPS->GetARChargeAttackComponent())
    {
      const FGameplayTag attackTag = CAC->EvaluateCharge(HeldTime, InTaskOwnerAbilityTag);
      const float nextChargeAttackTimeThreshold = CAC->GetNextChargeTimeThresholdByAttackTag(attackTag);
      const float nextChargeAttackCost = CAC->GetNextChargeAttackCostByAttackTag(attackTag);

      const float chargeAttackTimeThreshold = CAC->GetChargeTimeThresholdByAttackTag(attackTag);
      const float chargeAttackCost = CAC->GetChargeTimeThresholdByAttackTag(attackTag);

      
    }
  }
}

void AARRangerPlayerController::OnGameplayAbilityEnded_Hold(FGameplayTag InEndedAbilityTag, FGABlueprintableHoldHandle InHandle, float TimeHeld)
{
  ClearHoldSpec(InHandle.Handle);

  if (OnGameAbilityHeld.IsBound())
  {
    FGameplayTag nextAbilityTag = OnGameAbilityHeld.Execute(TimeHeld, InEndedAbilityTag);
    if (nextAbilityTag.IsValid())
    {
      // TODO Input buffer issue
      // NOTE: When we switch IMC, we clear all inputs.So this one will not be activated in some situation
      AbilityInputTagPressed(nextAbilityTag);
    }
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->OnHoldEnded();
  }
}

AARRangerPlayerController::FHoldSpec* AARRangerPlayerController::FindHoldSpecFromHandle(const FGA_HoldHandle& InHoldHandle) const
{
  auto searchPred = [&InHoldHandle](const FHoldSpec& Element)
  {
    return Element.Handle == InHoldHandle;
  };

  return const_cast<FHoldSpec*>(m_holdSpecs.FindByPredicate(searchPred));
}

void AARRangerPlayerController::ClearHoldSpec(const FGA_HoldHandle& InHoldHandle)
{
  FHoldSpec* foundSpec = FindHoldSpecFromHandle(InHoldHandle);
  if (foundSpec == nullptr)
  {
    return;
  }

  m_holdSpecs.RemoveSingle(*foundSpec);
  foundSpec = nullptr;
}

void AARRangerPlayerController::SwitchNextIMC(const FGameplayTag& InNextIMCTag)
{
  check(InputMappingContext != nullptr);
  if (InputMappingContext == nullptr)
  {
    return;
  }
  
  UInputMappingContext* nextIMC = InputMappingContext->FindIMCWithTag(InNextIMCTag);
  if (nextIMC != nullptr && CurrentIMC != nextIMC)
  {
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem != nullptr)
    {
      Subsystem->RemoveMappingContext(CurrentIMC);
      Subsystem->AddMappingContext(nextIMC, 0);
  
      CurrentIMC = nextIMC;

      // Clear input buffer
      if (InputBuffer != nullptr)
      {
        InputBuffer->ClearAllInputs();
      }
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

  // Bind native input actions
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_Move, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Move);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_Look, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Look);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_LockOn, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_ToggleLockOn);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_SwitchTarget_Left, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_SwitchTarget_Left);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_SwitchTarget_Right, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_SwitchTarget_Right);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_Transform, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Transform);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_Charge_Rotate, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_ChargeRotate);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_Target_Snap, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_TargetSnap);
  ARIC->BindNativeAction(InputConfig, ARRanger::GameplayTags::NativeInput_ResetCamera, ETriggerEvent::Started, this, &ThisClass::NativeInput_ResetCamera);

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

    InputBuffer->InitializeInputBuffer(InInputComponent, *InputConfig);
  }
}

void AARRangerPlayerController::AbilityInputTagPressed(FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  // If buffer is invalid then handle input,otherwise use input buffer to handle input 
  if (InputBuffer == nullptr)
  {
    if (UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(GetOwner()))
    {
      ARASC->AbilityInputTagPressed(InInputTag);
    }
  }
  else
  {
    InputBuffer->HandleInputTagPressed(InInputTag);
  }

}

void AARRangerPlayerController::AbilityInputTagReleased(FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  // If buffer is invalid then handle input,otherwise use input buffer to handle input 
  if (InputBuffer == nullptr)
  {
    if (UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(GetOwner()))
    {
      ARASC->AbilityInputTagReleased(InInputTag);
    }
  }
  else
  {
    InputBuffer->HandleInputTagReleased(InInputTag); 
  }
}

void AARRangerPlayerController::EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused)
{
  if (InputBuffer != nullptr)
  {
    InputBuffer->EvaluateBuffer(this, DeltaTime, bGamePaused);
  }
}

bool AARRangerPlayerController::IsInputBlocked(const FGameplayTag& InInputTag) const
{
  if (!InInputTag.IsValid())
  {
    return false;
  }

  // TODO Temporary
  // NOTE Maybe not only holdspec can block input 
  bool bInputBlocked = m_holdSpecs.Num() > 0;
  for (const FHoldSpec& holdSpec : m_holdSpecs)
  {
    if (holdSpec.Handle.IsValid() && holdSpec.InputBlockIgnoreTags.HasTagExact(InInputTag))
    {
      bInputBlocked = false;
      break;
    }
  }
  
  return bInputBlocked;

}

void AARRangerPlayerController::NativeInput_Move(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    const FVector2D moveInput = InputActionValue.Get<FVector2D>();
    OwningCharacter->DoMove(moveInput.X, moveInput.Y);
  }
  
}

void AARRangerPlayerController::NativeInput_JumpStart(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->DoJumpStart();
  }
}

void AARRangerPlayerController::NativeInput_JumpEnd(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->DoJumpEnd();
  }
}

void AARRangerPlayerController::NativeInput_Look(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    const FVector2D lookAxisVector = InputActionValue.Get<FVector2D>();
    OwningCharacter->DoLook(lookAxisVector.X, lookAxisVector.Y);
  }
}

void AARRangerPlayerController::NativeInput_ToggleLockOn(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    // TODO Uncomment it when we finish debugging
    // OwningCharacter->ToggleLockOn();
  }
}

void AARRangerPlayerController::NativeInput_SwitchTarget_Right(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->SwitchTargetRight();
  }
}

void AARRangerPlayerController::NativeInput_SwitchTarget_Left(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->SwitchTargetLeft();
  }
}

void AARRangerPlayerController::NativeInput_Transform(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->Transform();
  }
}

void AARRangerPlayerController::NativeInput_ChargeRotate(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    const float yawInput = InputActionValue.Get<float>();
    OwningCharacter->RotateCharacter_Charge(yawInput);
  }

}

void AARRangerPlayerController::NativeInput_TargetSnap(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    const FVector2D inputDir = InputActionValue.Get<FVector2D>();
    OwningCharacter->UpdateTargetSnap(inputDir);
  }
}

void AARRangerPlayerController::NativeInput_ResetCamera(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag)
{
  if (IsInputBlocked(InInputTag))
  {
    return;
  }

  if (OwningCharacter != nullptr)
  {
    OwningCharacter->ResetCamera();
  }
}
