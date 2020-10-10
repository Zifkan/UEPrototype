#pragma once
#include "CharacterActor.h"

struct RendererRoot
{
    int RenderEntityListId;
};

struct CharacterActorComponent
{
     ACharacterActor* Character;
};