#pragma once

#ifndef _AR_GAMEPLAY_TAGS_
#define _AR_GAMEPLAY_TAGS_

#include "NativeGameplayTags.h"

#define UE_API ARRANGER_API

namespace ARRanger
{

namespace GameplayTags
{
  extern UE_API FGameplayTag FindExactTagByString(const FString& InTagString);

  /**Ability Tags */
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo1);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo2);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo3);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Kick);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_HookShot);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cancel);

  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Charge_AttractionKick);

  /**InputState Tag */
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_BlockAll);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Default);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo1);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo2);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo3);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Kick);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Jump);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Charge_AttractionKick);

  /**Native Input Tag */
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Move);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_JumpStart);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_JumpEnd);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Look);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_LockOn);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_SwitchTarget_Left);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_SwitchTarget_Right);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Transform);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Charge_Rotate);

} // namespace ARRanger::GameplayTags;

} // namespace ARRanger

#endif // _AR_GAMEPLAY_TAGS_

#undef UE_API