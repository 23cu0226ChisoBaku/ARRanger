#pragma once

#include "EnemyAttackTypes.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
  None        UMETA(DisplayName = "None"),          
  Punch       UMETA(DisplayName = "Punch"),         
  Slammed     UMETA(DisplayName = "Slammed "),      
  JumpAttack  UMETA(DisplayName = "Jump Attack"),   
  Roar        UMETA(DisplayName = "Roar")           
};
