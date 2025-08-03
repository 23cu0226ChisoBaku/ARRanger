// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "ARPhysicsTickObject.generated.h"

struct FARPhysicsTickParameters;

struct FARPhysicsEvaluationResult
{
  ARRANGER_API explicit FARPhysicsEvaluationResult(const FVector& InPreviousForce);

  const FVector PreviousForce;
  
  FVector ForceResult; 
};


UCLASS(MinimalAPI, Abstract)
class UARPhysicsTickObject : public UObject
{
	GENERATED_BODY()

public:
  ARRANGER_API UARPhysicsTickObject();

  ARRANGER_API void BeginTickObject();
  ARRANGER_API void Tick(const FARPhysicsTickParameters& TickParams);
  ARRANGER_API void EndTickObject(const FARPhysicsEvaluationResult& EvaluatedResult);
  ARRANGER_API void TerminateTickObject();

  /**Start UObject interface */
  ARRANGER_API virtual void BeginDestroy() override;
  /**End UObject interface */

  ARRANGER_API virtual void OnBeginTickObject() { }
  ARRANGER_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) { }
  ARRANGER_API virtual void OnEndTickObject(const FARPhysicsEvaluationResult& EvaluatedResult) { }

  FVector GetLastFrameResult() const { return PreviousExternalForceResult; }
  FVector GetEvaluatedResult() const { return EvaluatedExternalForceResult; }
  bool IsCurrentFrameEvaluateFinished() const { return bIsCurrentFrameEvaluateFinished; }
  bool IsTerminated() const { return bIsTerminated; }

private:
  UPROPERTY(VisibleAnywhere)
  FVector PreviousExternalForceResult;

  UPROPERTY(VisibleAnywhere)
  FVector EvaluatedExternalForceResult;

  uint8 bIsTerminated : 1;
  uint8 bIsCurrentFrameEvaluateFinished : 1;

};
