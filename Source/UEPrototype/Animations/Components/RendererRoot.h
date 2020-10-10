#pragma once
#include "CharacterActor.h"

struct RendererRoot
{
    int RenderEntityListId;
};

struct RemoveRenderer
{
    int CustomRendererIndex;
};


struct CharacterActorComponent
{
     ACharacterActor* Character;
};