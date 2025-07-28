// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "IAudioPlayer.h"
#include "ARAudioSystem.generated.h"

class UAudioComponent;
class USoundBase;

USTRUCT(BlueprintType, DisplayName = "AR Sound Meta Data")
struct FARSoundMetaData : public FTableRowBase
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FString SoundID;

  UPROPERTY(EditAnywhere)
  TSoftObjectPtr<USoundBase> SoundAsset;
};

UCLASS()
class ARRANGER_API UARAudioSystem : public UGameInstanceSubsystem ,
                                    public FTickableGameObject,
                                    public IAudioPlayer
{
	GENERATED_BODY()

  protected:

    /**Start USubsystem interface */
      virtual void Initialize(FSubsystemCollectionBase& Collection) override;
      virtual void Deinitialize();
    /**End USubsystem interface */

    /**Start IAudioPlayer interface */
      virtual FSoundEffectHandle PlaySE3D(const FString& SEName, float Pitch, const FVector& Location) override;
      virtual FSoundEffectHandle PlaySE(const FString& SEName, float Pitch) override;
      virtual bool StopSE(const FSoundEffectHandle& SEHandle) override;
      virtual void PlayBGM(const FString& BGMName, float Pitch) override;
      virtual bool StopBGM() override;
    /**End IAudioPlayer interface */

    /**Start FTickableGameObject interface */
      virtual bool IsTickable() const override { return true; }
      virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UARAudioSystem, STATGROUP_Tickables); }
      virtual void Tick(float DeltaTime) override;
    /**End FTickableGameObject interface */

  public:
    // TODO: TEMP
    void InitializeSounds(UDataTable* BGMTable, UDataTable* SETable);

  private:
    UAudioComponent* PlaySE3DImpl(const FString& SEName, float Pitch, const FVector& Location);
    UAudioComponent* PlaySEImpl(const FString& SEName, float Pitch);
    void PlayBGMImpl(const FString& SEName, float Pitch);
    void SwitchBGM(USoundBase* NewBGMAsset);

    UPROPERTY()
    TMap< FString, TSoftObjectPtr< USoundBase > > m_bgmBuffer;

    UPROPERTY()
    TMap< FString, TSoftObjectPtr< USoundBase > > m_seBuffer;

    UPROPERTY()
    TObjectPtr<UAudioComponent> m_bgmComp;

  private:
    TArray<FSoundEffectHandle> m_seHandles;

    TArray<const FSoundEffectHandle*> m_requestRemoveHandles;
};
