// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARSoundPlayLibrary.generated.h"

USTRUCT(BlueprintType)
struct FARSoundBaseSettingParameters
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float Volume = 1.0f;

  UPROPERTY(EditAnywhere)
  float Pitch = 1.0f;

};

UENUM()
enum class EARSoundDimension
{
  Dimension2,
  Dimension3,
};

UENUM()
enum class EARSoundEffectType
{
  None,
  Jump,
  Attack,
  Dash,
};

USTRUCT(BlueprintType)
struct FARSoundEffectParameters : public FARSoundBaseSettingParameters
{
  GENERATED_BODY()

  TObjectPtr<AActor> RepositoryAccessActor;

  UPROPERTY(EditAnywhere)
  EARSoundDimension Dimension;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "Dimension == EARSoundDimension::Dimension3", EditConditionHides))
  FVector PlayLocation = FVector::ZeroVector;

  UPROPERTY(EditAnywhere)
  EARSoundEffectType SEType = EARSoundEffectType::None;

};

USTRUCT()
struct FARBackgroundMusicSoundDTO
{
  GENERATED_BODY()

  FString SoundID;

  FString AssetPath;

  float DefaultVolume;

  bool bIsLoop;
};


USTRUCT(BlueprintType)
struct FARBackgroundMusicParameters : public FARSoundBaseSettingParameters
{
  GENERATED_BODY()

  TObjectPtr<UObject> WorldContextObject;

  FString SoundID;
};

UCLASS()
class ARRANGER_API UARSoundPlayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, Category = "ARRanger|Sound")
  static void PlaySESound(const FARSoundEffectParameters& Parameters);

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Sound")
  static void PlayBGMSound(const FARBackgroundMusicParameters& Parameters);
	
};
