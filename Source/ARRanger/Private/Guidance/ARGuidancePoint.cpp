// Fill out your copyright notice in the Description page of Project Settings.


#include "Guidance/ARGuidancePoint.h"

#include "Components/BoxComponent.h"

// TODO
#include "ARRangerCharacter.h"

// Sets default values
AARGuidancePoint::AARGuidancePoint()
  : GuidancePointCollision{nullptr}
  , Visibility{EGuidancePointVisibilityType::Visible_Both}
  , bCanTerminationEventUpdate{false}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  GuidancePointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PointCollision"));
  GuidancePointCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnGuidancePointBeginOverlap);

}

// Called when the game starts or when spawned
void AARGuidancePoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void AARGuidancePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  if (GuidancePointCollision != nullptr)
  {
    GuidancePointCollision->OnComponentBeginOverlap.RemoveAll(this);
  }

  Super::EndPlay(EndPlayReason);
}

// Called every frame
void AARGuidancePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  if (bCanTerminationEventUpdate)
  {
    K2_OnTerminationEventUpdated(DeltaTime);
  }
}

void AARGuidancePoint::OnMagnetismTypeChanged(EARMagnetismType Type)
{
  using enum EARMagnetismType;
  switch (Type)
  {
    case Attraction:
    {
      K2_OnAttractionChanged();
    }
    break;

    case Repulsion:
    {
      K2_OnRepulsionChanged();
    }
    break;
  }
}

void AARGuidancePoint::SetActive(bool bActive)
{
  K2_OnActivationSet(bActive);
}

bool AARGuidancePoint::IsTerminationTriggered() const
{
  return bCanTerminationEventUpdate;
}

void AARGuidancePoint::OnTerminationEnded()
{
  K2_OnTerminationEnded();
}

void AARGuidancePoint::OnGuidancePointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (bCanTerminationEventUpdate)
  {
    return;
  }

  if (::Cast<AARRangerCharacter>(OtherActor) != nullptr)
  {
    K2_OnTerminationStarted();
    bCanTerminationEventUpdate = true;
  }
}



