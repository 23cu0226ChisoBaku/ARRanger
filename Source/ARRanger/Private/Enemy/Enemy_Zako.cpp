
#include "Enemy/Enemy_Zako.h"
#include "Enemy/ZakoAIController.h"

AEnemy_Zako::AEnemy_Zako()
{
    // AIController の指定（C++でもできる）
    AIControllerClass = AZakoAIController::StaticClass();

    // プレイヤーではないので自動制御をAIに
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}