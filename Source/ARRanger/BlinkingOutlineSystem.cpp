//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "BlinkingOutlineSystem.h"

AActor* BlinkingOutlineSystem::CreateTickingActor()
{
	UWorld* World = GetWorld();
	if (World && !TickActor)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return World->SpawnActor<ABlinkOutlineTickActor>(ABlinkOutlineTickActor::StaticClass(), FVector(0.f), FRotator::ZeroRotator, Params);
	}

	return nullptr;
}

void BlinkingOutlineSystem::RegisterOnOutline();

void BlinkingOutlineSystem::RequestOnBlinkingOutLine();
