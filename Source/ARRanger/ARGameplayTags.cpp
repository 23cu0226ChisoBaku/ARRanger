#include "ARGameplayTags.h"
#include "GameplayTagsManager.h"

namespace ARRanger
{

namespace GameplayTags
{
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo1, "Ability.Attack.Punch_Combo1", "Punch (Combo 1) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo2, "Ability.Attack.Punch_Combo2", "Punch (Combo 2) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Punch_Combo3, "Ability.Attack.Punch_Combo3", "Punch (Combo 3) attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Kick, "Ability.Attack.Kick", "Kick attack ability");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Special_HookShot, "Ability.Special.HookShot", "Attraction hook shot special attack ability");

  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Default, "InputState.Default", "Player default input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo1, "InputState.Punch.Combo1", "Player punch combo 1 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo2, "InputState.Punch.Combo2", "Player punch combo 2 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Punch_Combo3, "InputState.Punch.Combo3", "Player punch combo 3 input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Kick, "InputState.Kick", "Player kick input state");
  UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputState_Jump, "InputState.Jump", "Player jump input state");

  FGameplayTag FindExactTagByString(const FString& TagString)
  {
    const UGameplayTagsManager& manager = UGameplayTagsManager::Get();
    return manager.RequestGameplayTag(FName(*TagString), false);
  }
} // namespace ARRanger::GameplayTags

} // namespace ARRanger