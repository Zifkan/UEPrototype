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

	AnimationVertexMeshComponent = CreateDefaultSubobject<UAnimationVertexMeshComponent>(TEXT("Animation Vertex Mesh Component"));
	
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

	AnimationVertexMeshComponent =  CreateDefaultSubobject<UAnimationVertexMeshComponent>(TEXT("Mesh"));
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
//	characterEntity.set<FCharacterActorComponent>({this});
	
	for (int i = 0; i < BoneInfoArray.Num(); i++)
	{       
		auto entityBone = world->EntityManager->CreateEntity();

       
		em->SetComponentData<Translation>(entityBone,{});
		em->SetComponentData<LocalToWorld>(entityBone, {});    
		em->SetComponentData<CustomBone>(entityBone, { BoneInfoArray[i].BindMatrix });
		entityBone.add_childof(characterEntity);   
	}
}

void ACharacterActor::SetIndex(int index)
{
	
}

void ACharacterActor::SetSetBuffer(TArray<FMatrix> buffer)
{
	AnimationVertexMeshComponent->SetBuffer(buffer);
}

