// Unreal Engine �̃��W���[����`�t�@�C���̃w�b�_

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMStateMachineModule : public IModuleInterface
{
public:

	// IModuleInterface ���p��������K�{�I
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
