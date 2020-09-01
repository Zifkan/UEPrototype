// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Components/TransformComponents.h"
#include "UEPrototype/ECS/Components/CamComponent.h"
#include "UEPrototype/ECS/Components/MovementComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"


void AMainGameMode::BeginPlay()
{
 
    world = FEcsWorld::instance();
    auto em = world->EntityManager;

    em->RegisterComponent<CamComponent>("CamComponent");

    em->RegisterComponent<PlayerInputComponent>("InputComponent");
    em->RegisterComponent<MovementVelocity>("MovementVelocity");
    em->RegisterComponent<MoveDirectionData>("MoveDirectionData");
    em->RegisterComponent<MovementSpeed>("MovementSpeed"); 


    em->RegisterComponent<WorldToLocal>("WorldToLocal");
    em->RegisterComponent<Translation>("Translation");
    em->RegisterComponent<CopyTransformToActor>("CopyTransformToActor");
    em->RegisterComponent<Scale>("Scale");
    em->RegisterComponent<Rotation>("Rotation");
    em->RegisterComponent<FActorComponent>("ActorComponent");
   

    RegisterTransformSystems();
    RegisterSystem();
}

void AMainGameMode::RegisterSystem()
{
    systemsLauncher = new SystemLauncher(world);

  /*  systemsLauncher->RegisterSystem(new  CameraCollisionSystem());
    systemsLauncher->RegisterSystem(new  CameraSystem());
    systemsLauncher->RegisterSystem(new  PlayerViewDirectionSystem());
    systemsLauncher->RegisterSystem(new  InputMoveProcessingSystem());
    systemsLauncher->RegisterSystem(new  MovementVelocitySystem());
    systemsLauncher->RegisterSystem(new  CharacterRotationSystem());

    systemsLauncher->RegisterSystem(new  MovementCharacterSystem());
    systemsLauncher->RegisterSystem(new  CharacterIdleStateMachineSystem());
    systemsLauncher->RegisterSystem(new  CharacterStateMachineSystem());*/

}

void AMainGameMode::RegisterTransformSystems()
{
    transformLauncher = new TransformSystemLaunch(world);
}

void AMainGameMode::Tick(float DeltaSeconds)
{
    transformLauncher->Update(DeltaSeconds);
    systemsLauncher->Update(DeltaSeconds);
}
