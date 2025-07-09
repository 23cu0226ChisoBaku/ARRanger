// 内容：LogMState と LogMStateMachine というログカテゴリ実体（定義）を行っている

#include "MStateMachineLogChannels.h"

// ヘッダーの DECLARE_LOG_CATEGORY_EXTERN は「外部から使うための宣言」で、
// こちらの DEFINE_LOG_CATEGORY は「実体の定義」でセットで使う。
DEFINE_LOG_CATEGORY(LogMState);
DEFINE_LOG_CATEGORY(LogMStateMachine);