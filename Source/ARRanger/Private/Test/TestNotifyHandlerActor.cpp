// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestNotifyHandlerActor.h"
#include "PlayerObservation/INotifyHandlerInterface.h"

// Sets default values
ATestNotifyHandlerActor::ATestNotifyHandlerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATestNotifyHandlerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestNotifyHandlerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATestNotifyHandlerActor::TestJumpEvent()
{
  auto eventHandler = GetNotifyHandlerRef();
  eventHandler->OnJump();
}
