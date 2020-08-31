#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

struct FActorComponent
{
    TWeakObjectPtr<AActor> ptr;
};
