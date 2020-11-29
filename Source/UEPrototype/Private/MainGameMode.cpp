// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"


#include "Components/AnimComponent.h"
#include "Components/CustomBone.h"
#include "Components/TransformComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "GameFramework/PlayerInput.h"
#include "Systems/InputMoveProcessingSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Movement/MovementCharacterSystem.h"
#include "Systems/Movement/MovementVelocitySystem.h"
#include "Systems/Movement/PlayerViewDirectionSystem.h"
#include "UEPrototype/ECS/Components/CamComponent.h"
#include "UEPrototype/ECS/Components/MovementComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"


void AMainGameMode::BeginPlay()
{   
    world = FEcsWorld::instance();
    entityManager = world->EntityManager;

    entityManager->RegisterComponent<IdleState>("IdleState");
    entityManager->RegisterComponent<MoveState>("MoveState");
    entityManager->RegisterComponent<AnimComponent>("AnimComponent");
    
    

    entityManager->RegisterComponent<CamComponent>("CamComponent");
    entityManager->RegisterComponent<PlayerInputComponent>("InputComponent");
    entityManager->RegisterComponent<MovementVelocity>("MovementVelocity");
    entityManager->RegisterComponent<MoveDirectionData>("MoveDirectionData");
    entityManager->RegisterComponent<MovementSpeed>("MovementSpeed"); 
    entityManager->RegisterComponent<WorldToLocal>("WorldToLocal");
    entityManager->RegisterComponent<Translation>("Translation");
    entityManager->RegisterComponent<CopyTransformToActor>("CopyTransformToActor");
    entityManager->RegisterComponent<Scale>("Scale");
    entityManager->RegisterComponent<Rotation>("Rotation");
    entityManager->RegisterComponent<FActorComponent>("ActorComponent");
    entityManager->RegisterComponent<CustomBone>("CustomBone");
    entityManager->RegisterComponent<RendererRoot>("RendererRoot");
    entityManager->RegisterComponent<CharacterActorComponent>("CharacterActorComponent");
    entityManager->RegisterComponent<CopyTransformFromActor>("CopyTransformFromActor");
    entityManager->RegisterComponent<RemoveRenderer>("RemoveRenderer");
    

    RegisterTransformSystems();
    RegisterSystem();
}

void AMainGameMode::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
  //  PlayerInput->ProcessInputStack(InputComponent,DeltaTime,false);

    transformLauncher->Update(DeltaTime);    
    animationSystemLaunch->Update(DeltaTime);
    systemsLauncher->Update(DeltaTime);
    characterStateMachineLauncher->Update(DeltaTime);
}

void AMainGameMode::RegisterSystem()
{
    animationSystemLaunch = MakeUnique<AnimationSystemLaunch>(world);
    characterStateMachineLauncher = MakeUnique<CharacterStateMachineSystemLaunch>(world);


    
    systemsLauncher = MakeUnique<SystemLauncher>(world);


    systemsLauncher->RegisterSystem(new  CameraSystem());
    systemsLauncher->RegisterSystem(new  CameraCollisionSystem(), GetWorld());
    systemsLauncher->RegisterSystem(new  InputMoveProcessingSystem());      
    systemsLauncher->RegisterSystem(new  MovementVelocitySystem());
    systemsLauncher->RegisterSystem(new  PlayerViewDirectionSystem());
    systemsLauncher->RegisterSystem(new  MovementCharacterSystem());
    

}

void AMainGameMode::RegisterTransformSystems()
{
    transformLauncher = MakeUnique<TransformSystemLaunch>(world);
}
