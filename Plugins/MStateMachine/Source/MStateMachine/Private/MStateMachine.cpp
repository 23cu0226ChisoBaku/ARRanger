// モジュールの開始・終了時に行う処理をするクラス

#include "MStateMachine.h"

#define LOCTEXT_NAMESPACE "FMStateMachineModule"

void FMStateMachineModule::StartupModule()
{
	// このコードは、モジュールがメモリにロードされた後に実行されます。正確なタイミングは、モジュールごとの.upluginファイルで指定します。
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMStateMachineModule::ShutdownModule()
{
	// この関数は、モジュールをクリーンアップするためにシャットダウン中に呼ばれることがある。 動的リロードをサポートするモジュールの場合、
	// モジュールをアンロードする前にこの関数を呼び出す。
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMStateMachineModule, MStateMachine)