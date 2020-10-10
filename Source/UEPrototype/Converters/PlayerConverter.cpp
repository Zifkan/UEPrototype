// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"

#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"



void UPlayerConverter::Convert(flecs::entity e, FEntityManager dstManager)
{
    dstManager.AddComponentData<PlayerTag>(e);
    dstManager.AddComponentData<MovementVelocity>(e);
    dstManager.SetComponentData<MovementSpeed>(e, { 20.f });
    dstManager.AddComponentData<MoveDirectionData>(e);
    dstManager.AddComponentData<ViewDirectionData>(e);
    dstManager.AddComponentData<Rotation>(e);
    dstManager.AddComponentData<LocalToWorld>(e);
 //   dstManager.SetComponentData<Translation>(e, { FVector::ZeroVector });
    dstManager.AddComponentData<CopyTransformFromActor>(e);
    dstManager.SetComponentData<FActorComponent>(e, { GetOwner() });

}
