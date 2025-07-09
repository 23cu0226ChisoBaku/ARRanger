// Unreal Engine のモジュール定義ファイルのヘッダ

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMStateMachineModule : public IModuleInterface
{
public:

	// IModuleInterface を継承したら必須！
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
