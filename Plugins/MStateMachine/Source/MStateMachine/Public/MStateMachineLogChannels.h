// 内容： 他のコードファイルから「LogMState」「LogMStateMachine」という名前のログカテゴリを使えるように宣言

// インクルードガード
#pragma once

// UEのヘッダーファイル
// UEに関するログの機能を保持している
#include "Logging/LogMacros.h"

// LogMState と LogMStateMachine というログカテゴリの「外部宣言」をしている。

// DECLARE_LOG_CATEGORY_EXTERN は「外部から使うための宣言」であり、
// 実際の定義は別のcppファイルに DEFINE_LOG_CATEGORY(LogMState); のように書かれている。
// MSTATEMACHINE_API はモジュールの外部アクセスを許可するエクスポート / インポートマクロ。
// これにより他のモジュールからもこれらのログカテゴリを参照できる。

MSTATEMACHINE_API DECLARE_LOG_CATEGORY_EXTERN(LogMState, Log, All);
MSTATEMACHINE_API DECLARE_LOG_CATEGORY_EXTERN(LogMStateMachine, Log, All);