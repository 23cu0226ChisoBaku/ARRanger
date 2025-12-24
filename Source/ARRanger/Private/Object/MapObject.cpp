#include "Object/MapObject.h"

AMapObject::AMapObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMapObject::BeginPlay()
{
	Super::BeginPlay();
}

void AMapObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}