// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputPawn.h"
#include "MainPlayerController.h"
#include "Core/EcsWorld.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/AnimationSystemLaunch.h"
#include "Systems/SystemLauncher.h"
#include "Systems/StateMachine/CharacterStateMachineSystemLaunch.h"
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

        DefaultPawnClass = AInputPawn::StaticClass();
        // use our own player controller class
        PlayerControllerClass = AMainPlayerController::StaticClass();

    }       

protected:
    void BeginPlay() override;
    void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
private:
	GENERATED_BODY()

    void RegisterSystem();
    void RegisterTransformSystems();

private:

    FEcsWorld* world;
    TUniquePtr<AnimationSystemLaunch> animationSystemLaunch;
    TUniquePtr<SystemLauncher> systemsLauncher;
    TUniquePtr <TransformSystemLaunch> transformLauncher;
    TUniquePtr <CharacterStateMachineSystemLaunch> characterStateMachineLauncher;
    
    FEntityManager* entityManager;
};
