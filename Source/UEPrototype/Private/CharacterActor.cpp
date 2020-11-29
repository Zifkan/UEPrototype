// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActor.h"

#include "flecs.h"
#include "Components/CustomBone.h"
#include "Components/RendererRoot.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"

// Sets default values
ACharacterActor::ACharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

	AnimationVertexMeshComponent = CreateDefaultSubobject<UAnimVertexMeshComponent>(TEXT("Animation Vertex Mesh Component"));
	AnimationVertexMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);


}

// Called when the game starts or when spawned
void ACharacterActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto* world = FEcsWorld::instance();
	auto em = world->EntityManager;
	auto characterEntity = world->EntityManager->CreateEntity();

	characterEntity.add<RendererRoot>();
	characterEntity.set<CharacterActorComponent>({this});
	
	for (int i = 0; i < BoneInfoArray.Num(); i++)
	{       
		auto entityBone = world->EntityManager->CreateEntity();

       
		em->SetComponentData<Translation>(entityBone,{});
		em->SetComponentData<LocalToWorld>(entityBone, {});    
		em->SetComponentData<CustomBone>(entityBone, { BoneInfoArray[i].BindMatrix });
		entityBone.add_childof(characterEntity);   
	}

	
	StateTest();
}

void ACharacterActor::Tick(float DeltaSeconds)
{
	
}

void ACharacterActor::SetIndex(int index)
{
	
}

void ACharacterActor::SetSetBuffer(TArray<FMatrix> buffer)
{
	AnimationVertexMeshComponent->SetBuffer(buffer);
}

void ACharacterActor::StateTest()
{

	auto* world = FEcsWorld::instance();
	auto em = world->EntityManager;

	auto ecs = std::move(*world->DefaultWorld);

	/* Create a Movement state machine with 3 states */
	auto Walking = ecs.entity("Walking");
	auto Running = ecs.entity("Running");
	auto Movement = ecs.type("Movement", 
        "Walking, Running");

	/* Create a Direction state machine with 4 states */
	auto Front = ecs.entity("Front");
	auto Back = ecs.entity("Back");
	auto Left = ecs.entity("Left");
	auto Right = ecs.entity("Right");
	auto Direction = ecs.type("Direction", 
        "Front, Back, Left, Right");

	auto State =  ecs.type("State", 
        "Movement, Direction");

	/* Create a system that subscribes for all entities that have a Direction
	* and that are walking */
	ecs.system<>("Walk", "CASE | Walking, CASE | Right")
        .iter([](flecs::iter it)
        {
            /* Get the column with direction states. This is stored as an array
            * with identifiers to the individual states */
            flecs::column<flecs::entity_t> movement(it, 1);
            flecs::column<flecs::entity_t> direction(it, 2);

            for (auto i : it)
            {
            	UE_LOG(LogTemp, Warning, TEXT("%s; Movement = %s; Direction = %s"), UTF8_TO_TCHAR(it.entity(i).name().c_str()),
																					UTF8_TO_TCHAR(flecs::entity(it.world(), movement[i]).name().c_str()),
																					UTF8_TO_TCHAR(flecs::entity(it.world(), direction[i]).name().c_str()));
            }
        });

	/* Create a few entities with various state combinations */
	ecs.entity("e1")
	.add_switch(State)
           // .add_case(Movement)
        .add_switch(Movement)
            .add_case(Walking)
        .add_switch(Direction)
            .add_case(Front);

	ecs.entity("e2")
        .add_switch(Movement)
            .add_case(Running)
        .add_switch(Direction)
            .add_case(Left);

	ecs.entity("e3")
        .add_switch(Movement)
            .add_case(Walking)
        .add_switch(Direction)
            .add_case(Right);

	auto e4 = ecs.entity("e4")
        .add_switch(Movement)
            .add_case(Running)
        .add_switch(Direction)
            .add_case(Back);

	/* Add Walking to e4. This will remove the Running case */
	e4.add_case(Walking);

	ecs.set_target_fps(1);

	//while (ecs.progress()) { }
}

