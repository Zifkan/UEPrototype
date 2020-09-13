// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterRendererData.h"


#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"
#include "UEPrototype/Animations/Components/CustomBone.h"
#include "UEPrototype/Animations/Components/RendererRoot.h"

// Sets default values for this component's properties
UCharacterRendererData::UCharacterRendererData()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCharacterRendererData::BeginPlay()
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
        em->SetComponentData<CustomBone>(entityBone, { BoneInfoArray[i].GetBindMatrix() });
        entityBone.add_childof(characterEntity);   
    }
}
	

