#pragma once
#include "Math/Vector2D.h"

struct CombatMapData
{
    EInputEvent FirstAttack;
    EInputEvent SecondAttack;
    EInputEvent FirstAbility;
    EInputEvent SecondAbility;
};



struct PlayerInputComponent
{
    FVector2D MovementAxis;
    FVector2D AimAxis;

    EInputEvent Sprint;

    CombatMapData Combat;
};
