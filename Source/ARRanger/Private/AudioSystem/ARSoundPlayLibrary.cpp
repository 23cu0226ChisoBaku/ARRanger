// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSystem/ARSoundPlayLibrary.h"
#include "AudioKit.h"

// FIXME For player use only now
// FIXME Delete it when repository is ready
#include "ARRangerCharacter.h"


void UARSoundPlayLibrary::PlaySESound(const FARSoundEffectParameters& Parameters)
{
  if (Parameters.RepositoryAccessActor == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("Invalid use of UARSoundPlayLibrary::PlaySESound. Reason: Actor is nullptr"));
    return;
  }

  // FIXME Temporary code
  if (!Parameters.RepositoryAccessActor->GetClass()->IsChildOf<AARRangerCharacter>())
  {
    UE_LOG(LogTemp, Error, TEXT("TEMPORARY CODE!!!!!!!!!!!!!!!!!!!!!!!!!! Invalid use of UARSoundPlayLibrary::PlaySESound. Reason: Actor is not Player actor"));
    return;
  }

  using enum EARSoundDimension;
  using enum EARSoundEffectType;
  FString soundEffectID{};

  // TEMP CODE
  switch (Parameters.SEType)
  {
    case Jump:
    {
      soundEffectID.Append(TEXT("PlayerJump"));
    }
    break;
    case Attack:
    {
      soundEffectID.Append(TEXT("PlayerAttack"));
    }
    break;
    case Dash:
    {
      soundEffectID.Append(TEXT("PlayerDash"));
    }
    break;
  }

  switch (Parameters.Dimension)
  {
    // 2D
    case Dimension2:
    {
      MAudioKit::PlaySE(soundEffectID, Parameters.Pitch);
    }
    break;
    case Dimension3:
    {
      MAudioKit::PlaySE3D(soundEffectID, Parameters.Pitch, Parameters.PlayLocation);
    }
    break;
  }
}

void UARSoundPlayLibrary::PlayBGMSound(const FARBackgroundMusicParameters& Parameters)
{
  MAudioKit::PlayBGM(Parameters.SoundID, Parameters.Pitch);
}