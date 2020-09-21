// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActor.h"
#include "Components/CustomBone.h"
#include "Components/RendererRoot.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"

// Sets default values
ACharacterActor::ACharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

	Mesh =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ACharacterActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto* world = FEcsWorld::instance();
	auto em = world->EntityManager;
	auto characterEntity = world->EntityManager->CreateEntity();

	characterEntity.add<RendererRoot>();

	for (int i = 0; i < BoneInfoArray.Num(); i++)
	{       
		auto entityBone = world->EntityManager->CreateEntity();

       
		em->SetComponentData<Translation>(entityBone,{});
		em->SetComponentData<LocalToWorld>(entityBone, {});    
		em->SetComponentData<CustomBone>(entityBone, { BoneInfoArray[i].BindMatrix });
		entityBone.add_childof(characterEntity);   
	}
}
