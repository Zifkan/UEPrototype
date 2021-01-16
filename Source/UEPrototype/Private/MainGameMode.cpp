// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"


//#include "flecs-os_api-posix/include/flecs_os_api_posix.h"
#include "flecs_dash.h"
#include "flecs_systems_civetweb.h"
#include "Components/AnimComponent.h"
#include "Components/CustomBone.h"
#include "Components/TransformComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "GameFramework/PlayerInput.h"
#include "Systems/InputMoveProcessingSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Movement/MovementCharacterSystem.h"
#include "Systems/Movement/MovementVelocitySystem.h"
#include "Systems/Movement/PlayerViewDirectionSystem.h"
#include "Systems/StateMachine/ActionProceed/BaseActionProceedSystem.h"
#include "Systems/StateMachine/ActionProceed/BaseRemoveActionProceedSystem.h"
#include "UEPrototype/ECS/Components/CamComponent.h"
#include "UEPrototype/ECS/Components/MovementComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"


void AMainGameMode::BeginPlay()
{

   //posix_set_os_api();
    world = FEcsWorld::instance();
    
  /*  world->DefaultWorld->import<flecs::dash>();
    world->DefaultWorld->import<flecs::systems::civetweb>();

    world->DefaultWorld->entity().set<flecs::dash::Server>({9090}); */


    
    entityManager = world->EntityManager;
    
    entityManager->RegisterComponent<IdleState>("IdleState");
    entityManager->RegisterComponent<MoveState>("MoveState");
    entityManager->RegisterComponent<AnimComponent>("AnimComponent"); 
    entityManager->RegisterComponent<CheckActionData>("CheckActionData");
    entityManager->RegisterComponent<ActionAvailableTag>("ActionAvailableTag");

    entityManager->RegisterComponent<MoveInputTag>("MoveInputTag");
    entityManager->RegisterComponent<RollInputTag>("RollInputTag");
    entityManager->RegisterComponent<BlockInputTag>("BlockInputTag");
    entityManager->RegisterComponent<AttackInputTag>("AttackInputTag");
    entityManager->RegisterComponent<SprintInputTag>("SprintInputTag");


    entityManager->RegisterComponent<InheritsFrom>("InheritsFrom");
    entityManager->RegisterComponent<CamComponent>("CamComponent");
    entityManager->RegisterComponent<PlayerInputComponent>("PlayerInputComponent");
    entityManager->RegisterComponent<MovementVelocity>("MovementVelocity");
    entityManager->RegisterComponent<MoveDirectionData>("MoveDirectionData");
    entityManager->RegisterComponent<MovementSpeed>("MovementSpeed"); 
    entityManager->RegisterComponent<WorldToLocal>("WorldToLocal");
    entityManager->RegisterComponent<Translation>("Translation");
    entityManager->RegisterComponent<CopyTransformToActor>("CopyTransformToActor");
    entityManager->RegisterComponent<Scale>("Scale");
    entityManager->RegisterComponent<Rotation>("Rotation");
    entityManager->RegisterComponent<FActorComponent>("FActorComponent");
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
 //   characterStateMachineLauncher->Update(DeltaTime);
    removeActionProceedSystemLauncher->Update(DeltaTime);
}

void AMainGameMode::RegisterSystem()
{
    animationSystemLaunch = MakeUnique<AnimationSystemLaunch>(world);
 //   characterStateMachineLauncher = MakeUnique<CharacterStateMachineSystemLaunch>(world);
    systemsLauncher = MakeUnique<SystemLauncher>(world);
    removeActionProceedSystemLauncher = MakeUnique<RemoveActionProceedSystemLauncher>(world);
    
    

    systemsLauncher->RegisterSystem(new  CameraSystem());
    systemsLauncher->RegisterSystem(new  CameraCollisionSystem(), GetWorld());

	
    systemsLauncher->RegisterSystem(new  InputMoveProcessingSystem());

	systemsLauncher->RegisterSystem(new CheckActionAvailableSystem());

	
	systemsLauncher->RegisterSystem(new  SprintActionProceedSystem());	
	systemsLauncher->RegisterSystem(new  MoveActionProceedSystem());	
	systemsLauncher->RegisterSystem(new  AttackActionProceedSystem());

	systemsLauncher->RegisterSystem(new MovementStateSystem());
	systemsLauncher->RegisterSystem(new AttackStateSystem());       
	

	
    systemsLauncher->RegisterSystem(new  MovementVelocitySystem());
    systemsLauncher->RegisterSystem(new  PlayerViewDirectionSystem());

	systemsLauncher->RegisterSystem(new  MovementCharacterSystem());	

	
	
    
    
}

void AMainGameMode::RegisterTransformSystems()
{
    transformLauncher = MakeUnique<TransformSystemLaunch>(world);
}
