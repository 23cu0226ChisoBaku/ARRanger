// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UObject;
class AController;
class UMStateMachineComponent;
struct FGameplayTag;

struct FMStateContextInitializeParameters
{
  TObjectPtr<UObject> Owner;

  TObjectPtr<AController> OwnerController;

  TObjectPtr<const UMStateMachineComponent> StateMachineComponent;
};

class FMStateContext
{
  friend class UMStateMachineComponent;

  public:
    MSTATEMACHINE_API FMStateContext();
    MSTATEMACHINE_API ~FMStateContext();

  public:

    UObject* GetOwner() const { return m_weakOwner.Get(); }
    
    AController* GetOwnerController() const { return m_weakController.Get();}
    
    const UMStateMachineComponent* GetStateMachineComponent() const { return m_stateMachineComponent; }
    
    bool IsValid() const { return m_bIsValid; }
    
    MSTATEMACHINE_API UWorld* GetWorld() const;
    
    MSTATEMACHINE_API int32 GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const;
  
  private:
    MSTATEMACHINE_API void InitializeContext(const FMStateContextInitializeParameters& InitializeParams);

  private:
    TWeakObjectPtr<UObject> m_weakOwner;
    
    TWeakObjectPtr<AController> m_weakController;

    const UMStateMachineComponent* m_stateMachineComponent;

    bool m_bIsValid : 1;
};
