// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EcsWorld.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/SystemLauncher.h"
#include "Systems/Transforms/TransformSystemLaunch.h"


#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UEPROTOTYPE_API AMainGameMode : public AGameModeBase
{
public:
    AMainGameMode()
    {
        PrimaryActorTick.bStartWithTickEnabled = true;
        PrimaryActorTick.bCanEverTick = true;

    }

    void Tick(float DeltaSeconds) override;

protected:
    void BeginPlay() override;

private:
	GENERATED_BODY()

    void RegisterSystem();
    void RegisterTransformSystems();


private:

    FEcsWorld* world;
    SystemLauncher* systemsLauncher;
    TransformSystemLaunch* transformLauncher;

  
};
