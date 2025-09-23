// Fill out your copyright notice in the Description page of Project Settings.


#include "Guidance/ARGuidancePoint.h"

#include "Components/BoxComponent.h"

// TODO
#include "ARRangerCharacter.h"

// Sets default values
AARGuidancePoint::AARGuidancePoint()
  : GuidancePointCollision{nullptr}
  , ChildPoint{nullptr}
  , GuidanceAnimLength{1.0f}
  , GuidanceAnimDelay{0.2f}
  , m_guidanceAnimTimeCnt{0.0f}
  , m_guidanceAnimDelayTimerHandle{}
  , bLoopAnim{true}
  , bCanTerminationEventUpdate{false}
  , bIsAnimDelaying{false}
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

  if (m_guidanceAnimDelayTimerHandle.IsValid())
  {
    GetWorld()->GetTimerManager().ClearTimer(m_guidanceAnimDelayTimerHandle);
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

void AARGuidancePoint::SetChildPoint(AARGuidancePoint* InChild)
{
  if (InChild != this)
  {
    ChildPoint = InChild;
    OnChildPointSet(InChild);
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

bool AARGuidancePoint::CanPlayGuidanceAnimation() const
{
  return !bCanTerminationEventUpdate && (ChildPoint != nullptr);
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

void AARGuidancePoint::UpdateGuidanceAnimation(float DeltaTime)
{
  if (ChildPoint != nullptr)
  {
    OnGuidanceAnimationUpdated(*ChildPoint, DeltaTime);
  }
}

void AARGuidancePoint::ResetGuidanceAnimation()
{
  StopAnim();
  ResetAnim(); 
}

void AARGuidancePoint::OnGuidanceAnimationUpdated(const AARGuidancePoint& InChildPoint, float DeltaTime)
{
  if (&InChildPoint == this)
  {
    ResetAnim();
    return;
  }

  if (bIsAnimDelaying)
  {
    return;
  }

  const FVector guidanceDestination = InChildPoint.GetActorLocation();
  UpdateAnim(guidanceDestination, DeltaTime);
}

void AARGuidancePoint::ResetAnim()
{
  m_guidanceAnimTimeCnt = 0.0f;
  m_guidanceAnimDelayTimerHandle.Invalidate();
  bIsAnimDelaying = false;

  K2_OnGuidanceAnimationReset();
}

void AARGuidancePoint::UpdateAnim(const FVector& Destination, float DeltaTime)
{
  m_guidanceAnimTimeCnt += DeltaTime;
  m_guidanceAnimTimeCnt = FMath::Clamp(m_guidanceAnimTimeCnt, 0.0f, GuidanceAnimLength);

  // Update Animation On BP
  K2_OnGuidanceAnimationUpdated(GetActorLocation(), Destination, GuidanceAnimLength, m_guidanceAnimTimeCnt, DeltaTime);

  if (FMath::IsNearlyEqual(m_guidanceAnimTimeCnt, GuidanceAnimLength))
  {
    // Stop Animation
    StopAnim();

    if (!FMath::IsNearlyZero(GuidanceAnimDelay))
    {
      bIsAnimDelaying = true;

      auto delayHandler = [this]()
      {
        this->ResetAnim();
      };

      if (!m_guidanceAnimDelayTimerHandle.IsValid())
      {
        GetWorld()->GetTimerManager().SetTimer(m_guidanceAnimDelayTimerHandle, delayHandler, GuidanceAnimDelay, /**bLoop */ false);
      }
    }
  }
}

void AARGuidancePoint::StopAnim()
{
  K2_OnGuidanceAnimationEnded();
}

void AARGuidancePoint::OnChildPointSet(AARGuidancePoint* InChildPoint)
{
  // Reset parent animation when parent changed
  if (InChildPoint != nullptr)
  {
    ResetAnim();
    K2_OnChildPointSet(InChildPoint);
  }
}


