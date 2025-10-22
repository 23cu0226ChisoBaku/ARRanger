#include "ARGameplayTags.h"
#include "GameplayTagsManager.h"

namespace ARRanger
{

namespace GameplayTags
{
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch, "Ability.Attack.Punch", "Standard punch attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_JumpAttack, "Ability.Attack.JumpAttack", "Jump attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo1, "Ability.Attack.Punch_Combo1", "Punch (Combo 1) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo2, "Ability.Attack.Punch_Combo2", "Punch (Combo 2) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo3, "Ability.Attack.Punch_Combo3", "Punch (Combo 3) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_RepulsionKick_Weak, "Ability.Attack.RepulsionKick.Weak", "RepulsionKick (Weak) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_RepulsionKick_Moderate, "Ability.Attack.RepulsionKick.Moderate", "RepulsionKick (Moderate) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_RepulsionKick_Strong, "Ability.Attack.RepulsionKick.Strong", "RepulsionKick (Strong) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_HookShot, "Ability.Special.HookShot", "Attraction hook shot special attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_StrongPunch, "Ability.Special.StrongPunch", "Strong punch special attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cancel, "Ability.Cancel", "Tag to cancel abilities");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Jump, "Ability.Jump", "Tag to activate jump ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Charge_RepulsionKick, "Ability.Charge.RepulsionKick", "Repulsion kick charge ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Charge_Punch_Combo3, "Ability.Charge.Punch_Combo3", "Punch (Combo 3) charge ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cancel_OnDamaged, "Ability.Cancel.OnDamaged", "Tag to cancel abilities if avatar is damaged");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dead, "Ability.Dead", "Tag to handle dead event");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_Attract_StartPhase, "Ability.Special.AttractStartPhase", "Player Attract Special Start");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_Attract_AttackPhase, "Ability.Special.AttractAttackPhase", "Player Attract Special Attack");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_Attract_EndPhase, "Ability.Special.AttractEndPhase", "Player Attract Special End");

  UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityCost_ARBurst, "AbilityCost.ARBurst", "Resource for Special Action");

  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_BlockAll, "InputState.BlockAll", "Block all player input");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Default, "InputState.Default", "Player default input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo1, "InputState.Punch.Combo1", "Player punch combo 1 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo2, "InputState.Punch.Combo2", "Player punch combo 2 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo3, "InputState.Punch.Combo3", "Player punch combo 3 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Kick, "InputState.Kick", "Player kick input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_OnAir, "InputState.OnAir", "Player on air input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Charge_RepulsionKick, "InputState.Charge.RepulsionKick", "Player repulsion charge input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Charge_Punch_Combo3, "InputState.Charge.Punch_Combo3", "Player punch (Combo 3) charge input state");

  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_Move, "NativeInput.Move", "Character movement input");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_Look, "NativeInput.Look", "Character look input");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_LockOn, "NativeInput.LockOn", "Character lock on target input");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_SwitchTarget_Left, "NativeInput.SwitchTarget.Left", "Character switch lock on target(Left)");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_SwitchTarget_Right, "NativeInput.SwitchTarget.Right", "Character switch lock on target(Right)");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_Transform, "NativeInput.Transform", "Character transform input");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_Charge_Rotate, "NativeInput.Charge.Rotate", "Character can rotate during charging");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_Target_Snap, "NativeInput.TargetSnap", "Character can snap to target in input direction");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(NativeInput_ResetCamera, "NativeInput.ResetCamera", "Reset player camera to default location");

  FGameplayTag FindExactTagByString(const FString& TagString)
  {
    const UGameplayTagsManager& manager = UGameplayTagsManager::Get();
    return manager.RequestGameplayTag(FName(*TagString), false);
  }
} // namespace ARRanger::GameplayTags

} // namespace ARRanger