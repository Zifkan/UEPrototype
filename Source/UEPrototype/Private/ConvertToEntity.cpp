// Fill out your copyright notice in the Description page of Project Settings.


#include "ConvertToEntity.h"

#include "Core/EcsWorld.h"

// Called when the game starts
void UConvertToEntity::BeginPlay()
{
	Super::BeginPlay();

    auto* world = FEcsWorld::instance();
    auto e = world->EntityManager->CreateEntity(StringCast<ANSICHAR>(*EntityName).Get());
    Convert(e, *world->EntityManager);
}