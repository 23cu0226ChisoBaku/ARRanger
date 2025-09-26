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
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_JumpAttack);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo1);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo2);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Punch_Combo3);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_RepulsionKick_Weak);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_RepulsionKick_Moderate);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_RepulsionKick_Strong);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_HookShot);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_StrongPunch);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cancel);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Jump);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cancel_OnDamaged);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dead);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_Attract_StartPhase);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_Attract_AttackPhase);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Special_Attract_EndPhase);

  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Charge_RepulsionKick);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Charge_Punch_Combo3);

  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityCost_ARBurst);

  /**InputState Tag */
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_BlockAll);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Default);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo1);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo2);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Punch_Combo3);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Kick);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_OnAir);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Charge_RepulsionKick);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputState_Charge_Punch_Combo3);

  /**Native Input Tag */
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Move);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Look);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_LockOn);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_SwitchTarget_Left);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_SwitchTarget_Right);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Transform);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Charge_Rotate);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_Target_Snap);
  UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NativeInput_ResetCamera);

} // namespace ARRanger::GameplayTags;

} // namespace ARRanger

#endif // _AR_GAMEPLAY_TAGS_

#undef UE_API